#ifndef EDUCATIVEMODERNCPPCONCUR_THREADSAFELINKEDLIST_H
#define EDUCATIVEMODERNCPPCONCUR_THREADSAFELINKEDLIST_H
#include <iostream>
#include <atomic>
#include <memory> //for shared_ptr
#include <typeinfo>
namespace ThreadSafeLinkedList
{
    using namespace std;

    template<typename T>
    class concurrent_stack
    {
        struct Node
        {
            T t;
            shared_ptr<Node> next;
        };
        shared_ptr<Node> m_head;

        concurrent_stack(concurrent_stack &) = delete;
        void operator=(concurrent_stack &) = delete;

    public:
        concurrent_stack() = default;
        ~concurrent_stack() = default;

        class reference //for returning purpose in find() and front(), so it should be public
        {
            shared_ptr<Node> p;
        public:
            reference(shared_ptr<Node> p_) : p{p_} { }
            T & operator*() { return p->t; }
            T * operator->() { return &p->t; }
        };

        auto find(T t) const
        {
            auto p = atomic_load(&m_head);
            std::cout << typeid(p).name() << std::endl;
            while (p && p->t != t)
                p = p->next;
            return reference(move(p)); //return a reference object that wraps shared_ptr<Node>
            //instead of returning a shared_ptr<Node>
        }

        auto front() const
        {
            return reference(atomic_load(&m_head)); //return a reference object that wraps shared_ptr<Node>
            //instead of returning a shared_ptr<Node>
        }

        void push_front(T t)
        {
            auto p = make_shared<Node>();
            p->t = t;

            //now make p the new head
            p->next = atomic_load(&m_head); //std::memory_order_seq_cst
            //but if m_head is no longer what's stored in p->next (atomic_compare_exchange_weak return false)
            //(some other thread must have inserted a node just now)
            //then p->next will be set to m_head in the same atomic operation
            //do this in a while loop for weak version for better performance
            //(weak version means: it may return false when obj==expected, so it requires a while loop)
            //until it return true, and in the same atomic operation, m_head will be set to p
            //in short: compare m_head to p->next, set p->next to m_head if false, set m_head to p if true, in an atomic operation
            while (p && !atomic_compare_exchange_weak(&m_head, &p->next, p))
            {} //std::memory_order_seq_cst
            // bool atomic_compare_exchange_weak(atomic<T>* obj, atomic<T>* expected, atomic<T> desired)
            // if (obj==expected) return true, obj will be set in the same atomic operation to desired
            // if (obj==expected) return false, expected will be set in the same atomic operation to obj
            // atomic_compare_exchange might suffer from the ABA problem
        }

        void pop_front()
        {
            auto p = atomic_load(&m_head);
            std::cout << typeid(p).name() << std::endl;
            //compare m_head to p, set p to m_head if false, set m_head to p->next if true, in an atomic operation
            while (p && !atomic_compare_exchange_weak(&m_head, &p, p->next))
            {}
        }

    };

    void Run()
    {
        concurrent_stack<int> cS;
        cS.push_front(3);
        cS.push_front(6);
        auto found = cS.find(6);
        std::cout << typeid(found).name() << std::endl;
        std::cout << "Find: " << *found << std::endl;
        cS.pop_front();
        auto front = cS.front();
        std::cout << typeid(front).name() << std::endl;
        std::cout << "Front: " << *front << std::endl;
    }

/*
template<typename T>
class concurrent_stack_
{
    struct Node
    {
        T t;
        shared_ptr<Node> next;
    };
    atomic_shared_ptr<Node> head;
    // in C++11: remove "atomic_" and remember to use the special.
    // functions every time you touch the variable
    concurrent_stack_(concurrent_stack_ &) = delete;
    void operator=(concurrent_stack_ &) = delete;

public:
    concurrent_stack_() = default;
    ~concurrent_stack_() = default;

    class reference
    {
        shared_ptr<Node> p;
    public:
        reference(shared_ptr<Node> p_) : p{p_} { }
        T& operator* () {return p->t;}
        T* operator->() {return &p->t;}
    };

    auto find( T t) const
    {
        auto p = head.load(); // in C++11: atomic_load(&head)
        while(p && p->t != t)
            p = p->next;
        return reference(move(p));
    }

    auto front() const
    {
        return reference(head); // in C++11: atomic_load(&head)
    }

    void push_front(T t)
    {
        auto p = make_shared<Node>();
        p->t = t;
        p->next = head;       // in C++11: atomic_load(&head)
        while(!head.compare_exchange_weak(p->next, p)){ }
        // in C++11: atomic_compare_exchange_weak(&head, &p->next, p)
    }

    void pop_front()
    {
        auto p = head.load(); // in C++11: atomic_load(&head)
        while(p && !head.compare_exchange_weak(p, p->next){ }
        // in C++11: atomic_compare_exchange_weak(&head, &p, p->next)
    }

};
*/
}
#endif //EDUCATIVEMODERNCPPCONCUR_THREADSAFELINKEDLIST_H
