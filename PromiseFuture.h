#ifndef EDUCATIVEMODERNCPPCONCUR_PROMISEFUTURE_H
#define EDUCATIVEMODERNCPPCONCUR_PROMISEFUTURE_H
#include <future>
#include <iostream>
#include <thread>
#include <utility>

namespace PromiseFuture
{
    void product(std::promise<int> & intPromise, int a, int b)
    {
        intPromise.set_value(a * b);
    }

    struct Div
    {

        void operator()(std::promise<int> & intPromise, int a, int b) const
        {
            intPromise.set_value(a / b);
        }

    };

    void Run()
    {

        int a = 20;
        int b = 10;

        std::cout << std::endl;

        // define the promises
        std::promise<int> prodPromise;
        std::promise<int> divPromise;

        // get the futures
        std::future<int> prodResult = prodPromise.get_future();
        std::future<int> divResult = divPromise.get_future();

        // calculate the result in a separate thread
        std::thread prodThread(product, std::ref(prodPromise), a, b);//invoke a global function
        Div div;
        std::thread divThread(div, std::ref(divPromise), a, b);//invoke a function object

        // get the result
        std::cout << "20*10 = " << prodResult.get() << std::endl;
        std::cout << "20/10 = " << divResult.get() << std::endl;

        prodThread.join();

        divThread.join();

        std::cout << std::endl;

    }
}
#endif //EDUCATIVEMODERNCPPCONCUR_PROMISEFUTURE_H
