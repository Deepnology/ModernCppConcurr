#ifndef MODERNCPPCONCURR_ACQUIRERELEASEWITHWAITING_H
#define MODERNCPPCONCURR_ACQUIRERELEASEWITHWAITING_H
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
namespace AcquireReleaseWithWaiting
{
    std::vector<int> mySharedWork;
    std::atomic<bool> dataProduced(false);

    void dataProducer()
    {
        mySharedWork = {1, 0, 3};
        dataProduced.store(true, std::memory_order_release);
    }

    void dataConsumer()
    {
        while (!dataProduced.load(std::memory_order_acquire));
        mySharedWork[1] = 2;
    }

    void Run()
    {

        std::cout << std::endl;

        std::thread t1(dataConsumer);
        std::thread t2(dataProducer);

        t1.join();
        t2.join();

        for (auto v: mySharedWork)
        {
            std::cout << v << " ";
        }

        std::cout << "\n\n";

    }
}
#endif //MODERNCPPCONCURR_ACQUIRERELEASEWITHWAITING_H
