#ifndef MODERNCPPCONCURR_DEADLOCKRESOLVED_H
#define MODERNCPPCONCURR_DEADLOCKRESOLVED_H
#include <iostream>
#include <chrono>
#include <mutex>
#include <thread>
namespace DeadlockResolved
{
    using namespace std;

    struct CriticalData
    {
        std::mutex mut;
    };

    void deadLock1(CriticalData& a, CriticalData& b)
    {
        a.mut.lock();
        std::cout << "Thread: " << std::this_thread::get_id() << " get the first mutex" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        b.mut.lock();
        std::cout << "Thread: " << std::this_thread::get_id() << " get the second mutex" << std::endl;
        // do something with a and b
        a.mut.unlock();
        b.mut.unlock();

    }

    void deadLock2(CriticalData& a, CriticalData& b)
    {
        unique_lock<mutex> guard1(a.mut,defer_lock);
        cout << "Thread: " << this_thread::get_id() << " first mutex" << endl;

        this_thread::sleep_for(chrono::milliseconds(1));

        unique_lock<mutex> guard2(b.mut,defer_lock);
        cout << "  Thread: " << this_thread::get_id() << " second mutex" <<  endl;

        cout << "    Thread: " << this_thread::get_id() << " get both mutex" << endl;
        lock(guard1, guard2);
        // do something with a and b
    }

    void deadLock3(CriticalData& a, CriticalData& b)
    {
        lock_guard<std::mutex> guard1(a.mut, std::adopt_lock);//adopt_lock: assume the calling thread already has ownership of the mutex
        cout << "Thread: " << this_thread::get_id() << " first mutex" << endl;

        this_thread::sleep_for(chrono::milliseconds(1));

        lock_guard<std::mutex> guard2(b.mut, std::adopt_lock);//adopt_lock: assume the calling thread already has ownership of the mutex
        cout << "  Thread: " << this_thread::get_id() << " second mutex" <<  endl;

        cout << "    Thread: " << this_thread::get_id() << " get both mutex" << endl;
        lock(a.mut, b.mut);
        // do something with a and b
    }

    void deadLock4(CriticalData& a, CriticalData& b)
    {

        cout << "Thread: " << this_thread::get_id() << " first mutex" << endl;
        this_thread::sleep_for(chrono::milliseconds(1));
        cout << "  Thread: " << this_thread::get_id() << " second mutex" <<  endl;
        cout << "    Thread: " << this_thread::get_id() << " get both mutex" << endl;

        std::scoped_lock(a.mut, b.mut);
        // do something with a and b
    }

    int Run()
    {
        /*
        {
            CriticalData c1;
            CriticalData c2;
            std::thread t1([&]{deadLock1(c1,c2);});
            std::thread t2([&]{deadLock1(c2,c1);});
            t1.join();
            t2.join();
        }
        */
        {
            CriticalData c1;
            CriticalData c2;
            std::thread t1([&]{deadLock2(c1,c2);});
            std::thread t2([&]{deadLock2(c2,c1);});
            t1.join();
            t2.join();
        }
        {
            CriticalData c1;
            CriticalData c2;
            std::thread t1([&]{deadLock3(c1,c2);});
            std::thread t2([&]{deadLock3(c2,c1);});
            t1.join();
            t2.join();
        }
        {
            CriticalData c1;
            CriticalData c2;
            std::thread t1([&]{deadLock4(c1,c2);});
            std::thread t2([&]{deadLock4(c2,c1);});
            t1.join();
            t2.join();
        }
    }
}
#endif //MODERNCPPCONCURR_DEADLOCKRESOLVED_H
