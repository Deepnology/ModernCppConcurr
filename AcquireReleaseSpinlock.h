#ifndef MODERNCPPCONCURR_ACQUIRERELEASESPINLOCK_H
#define MODERNCPPCONCURR_ACQUIRERELEASESPINLOCK_H
#include<iostream>
#include <atomic>
#include <thread>
namespace AcquireReleaseSpinlock
{
    class Spinlock
    {
        std::atomic_flag flag;
    public:
        Spinlock(): flag(ATOMIC_FLAG_INIT) {}

        void lock()
        {
            while(flag.test_and_set(std::memory_order_acquire));
        }

        void unlock()
        {
            flag.clear(std::memory_order_release);
        }
    };

    Spinlock spin;
    void workOnResource()
    {
        spin.lock();
        // shared resource
        spin.unlock();
        std::cout << "Work done" << std::endl;
    }


    void Run()
    {
        std::thread t(workOnResource);
        std::thread t2(workOnResource);

        t.join();
        t2.join();
    }
}
#endif //MODERNCPPCONCURR_ACQUIRERELEASESPINLOCK_H
