#ifndef MODERNCPPCONCURR_SHAREDFUTURE_H
#define MODERNCPPCONCURR_SHAREDFUTURE_H
#include <future>
#include <iostream>
#include <thread>
#include <utility>
namespace SharedFuture
{
    std::mutex coutMutex;

    struct Div
    {

        void operator()(std::promise<int> && intPromise, int a, int b)
        {
            intPromise.set_value(a / b);
        }

    };

    struct Requestor
    {

        void operator()(std::shared_future<int> shaFut)
        {

            // lock std::cout
            std::lock_guard<std::mutex> coutGuard(coutMutex);

            // get the thread id
            std::cout << "threadId(" << std::this_thread::get_id() << "): ";

            std::cout << "20/10= " << shaFut.get() << std::endl;

        }

    };

    void Run()
    {

        std::cout << std::endl;

        // define the promises
        std::promise<int> divPromise;

        // get the futures
        std::shared_future<int> divResult = divPromise.get_future();

        // calculate the result in a separat thread
        Div div;
        std::thread divThread(div, std::move(divPromise), 20, 10);//invoke a function object

        Requestor req;
        std::thread sharedThread1(req, divResult);//shared_future is passed by value
        std::thread sharedThread2(req, divResult);//shared_future is passed by value
        std::thread sharedThread3(req, divResult);//shared_future is passed by value
        std::thread sharedThread4(req, divResult);//shared_future is passed by value
        std::thread sharedThread5(req, divResult);//shared_future is passed by value

        divThread.join();

        sharedThread1.join();
        sharedThread2.join();
        sharedThread3.join();
        sharedThread4.join();
        sharedThread5.join();

        std::cout << std::endl;

    }
}
#endif //MODERNCPPCONCURR_SHAREDFUTURE_H
