#ifndef _ATOMIC_STACK_H
#define _ATOMIC_STACK_H
#include <iostream>
#include <atomic>
#include <memory>
namespace atomic_stack
{
    template<typename T>
    class AtomicStack
    {
        struct Node
        {
            T t;
            std::shared_ptr<Node> next;
        };
        std::shared_ptr<Node> m_head;

        AtomicStack(const AtomicStack &) = delete;
        AtomicStack(const AtomicStack &&) = delete;
        AtomicStack &operator=(const AtomicStack &) = delete;
        AtomicStack &operator=(const AtomicStack &&) = delete;

    public:
        AtomicStack() = default;
        ~AtomicStack() = default;

        class reference //for returning purpose in find() and front(), so it should be public
        {
            std::shared_ptr<Node> p;
        public:
            reference(std::shared_ptr<Node> p_) : p{p_} { }
            T & operator*() { return p->t; }
            T * operator->() { return &p->t; }
        };

        auto Top() const
        {
            return reference(std::atomic_load(&m_head)); //std::memory_order_seq_cst
            //return a reference object that wraps shared_ptr<Node>
            //instead of returning a shared_ptr<Node>
        }

        void Push(T t)
        {
            auto p = std::make_shared<Node>();
            p->t = t;

            //now make p the new head
            p->next = std::atomic_load(&m_head); //std::memory_order_seq_cst
            //but if m_head is no longer what's stored in p->next (atomic_compare_exchange_weak return false)
            //(some other thread must have inserted a node just now)
            //then p->next will be set to m_head in the same atomic operation
            //do this in a while loop with weak version for better performance
            //(weak version means: it may return false when obj==expected, so it requires a while loop)
            //until it return true, and in the same atomic operation, m_head will be set to p
            //in short: compare m_head to p->next, set p->next to m_head if false, set m_head to p if true, in an atomic operation
            while (p && !std::atomic_compare_exchange_weak(&m_head, &p->next, p)) //std::memory_order_seq_cst
            {}
            // bool atomic_compare_exchange_weak(atomic<T>* obj, atomic<T>* expected, atomic<T> desired)
            // if (obj==expected) return true, obj will be set in the same atomic operation to desired
            // if (obj==expected) return false, expected will be set in the same atomic operation to obj
            // atomic_compare_exchange might suffer from the ABA problem
        }

        void Pop()
        {
            auto p = std::atomic_load(&m_head); //std::memory_order_seq_cst

            //compare m_head to p, set p to m_head if false, set m_head to p->next if true, in an atomic operation
            while (p && !std::atomic_compare_exchange_weak(&m_head, &p, p->next)) //std::memory_order_seq_cst
            {}

            //atomic_compare_exchange might suffer from the ABA problem:
            //if we use raw pointers instead of shared_ptr,
            //p (prev_head) might already been deleted by another thread in Pop()
        }

    };

    void Run()
    {
        AtomicStack<int> s;
        s.Push(3);
        s.Push(6);
        s.Pop();

        std::cout << "Top: " << *s.Top() << std::endl;
    }
}
#endif
