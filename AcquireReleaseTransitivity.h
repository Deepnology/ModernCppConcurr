#ifndef MODERNCPPCONCURR_ACQUIRERELEASETRANSITIVITY_H
#define MODERNCPPCONCURR_ACQUIRERELEASETRANSITIVITY_H
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
namespace AcquireReleaseTransitivity
{
    std::vector<int> mySharedWork;
    std::atomic<bool> dataProduced(false);
    std::atomic<bool> dataConsumed(false);

    void dataProducer()
    {
        mySharedWork = {1,0,3};
        dataProduced.store(true, std::memory_order_release);
    }

    void deliveryBoy()
    {
        while(!dataProduced.load(std::memory_order_acquire));
        dataConsumed.store(true, std::memory_order_release);
    }

    void dataConsumer()
    {
        while(!dataConsumed.load(std::memory_order_acquire));
        mySharedWork[1] = 2;
    }

    void Run()
    {

        std::cout << std::endl;

        std::thread t1(dataConsumer);
        std::thread t2(deliveryBoy);
        std::thread t3(dataProducer);

        t1.join();
        t2.join();
        t3.join();

        for (auto v: mySharedWork){
            std::cout << v << " ";
        }

        std::cout << "\n\n";

    }
}
#endif //MODERNCPPCONCURR_ACQUIRERELEASETRANSITIVITY_H
