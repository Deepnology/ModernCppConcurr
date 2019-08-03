#ifndef MODERNCPPCONCURR_ATOMICMEMMODELCASE1_H
#define MODERNCPPCONCURR_ATOMICMEMMODELCASE1_H
#include <atomic>
#include <iostream>
#include <thread>
namespace AtomicMemModelCase1
{
    namespace sequential_consistency
    {
        std::atomic_int x, y;
        int r1, r2;
        void writeX()
        {
            x.store(1, std::memory_order_seq_cst);
            r1 = y.load(std::memory_order_seq_cst);
        }
        void writeY()
        {
            y.store(1, std::memory_order_seq_cst);
            r2 = x.load(std::memory_order_seq_cst);
        }
        void Run()
        {
            x = 0;
            y = 0;
            std::thread a(writeX);
            std::thread b(writeY);
            a.join();
            b.join();
            std::cout << "sequential consistency: " << r1 << ", " << r2 << std::endl;
        }
    }
    namespace acquire_release
    {
        std::atomic_int x, y;
        int r1, r2;
        void writeX()
        {
            x.store(1, std::memory_order_relaxed);
            r1 = y.load(std::memory_order_acquire);
        }
        void writeY()
        {
            y.store(1, std::memory_order_release);
            r2 = x.load(std::memory_order_relaxed);
        }
        void Run()
        {
            x = 0;
            y = 0;
            std::thread a(writeX);
            std::thread b(writeY);
            a.join();
            b.join();
            std::cout << "acquire release: " << r1 << ", " << r2 << std::endl;
            /*
             * 3 ways of execution order
             * r1 r2
             * 0  1    x.store, y.load y.store x.load
             * 0  0    impossible
             * 1  1    y.store x.store y.load x.load
             * 1  0    y.store x.load x.store y.load
             */
        }
    }
    namespace relaxed
    {
        std::atomic_int x, y;
        int r1, r2;
        void writeX()
        {
            x.store(1, std::memory_order_relaxed);
            r1 = y.load(std::memory_order_relaxed);
        }
        void writeY()
        {
            y.store(1, std::memory_order_relaxed);
            r2 = x.load(std::memory_order_relaxed);
        }
        void Run()
        {
            x = 0;
            y = 0;
            std::thread a(writeX);
            std::thread b(writeY);
            a.join();
            b.join();
            std::cout << "relaxed: " << r1 << ", " << r2 << std::endl;
            /*
             * all permutations = 4*3*2*1 = 24 ways of execution order
             * r1 r2
             * 0  1
             * 0  0
             * 1  1
             * 1  0
             */
        }
    }
}
#endif //MODERNCPPCONCURR_ATOMICMEMMODELCASE1_H
