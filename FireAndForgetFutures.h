#ifndef MODERNCPPCONCURR_FIREANDFORGETFUTURES_H
#define MODERNCPPCONCURR_FIREANDFORGETFUTURES_H
#include <chrono>
#include <future>
#include <iostream>
#include <thread>
namespace FireAndForgetFutures
{
    void Run()
    {

        std::cout << std::endl;

        //fire and forget:
        std::async(std::launch::async, []{
            std::this_thread::sleep_for(std::chrono::seconds(5));
            std::cout << "first thread" << std::endl;
        });//block: a future that is created by std::async waits on its dtor until its promise is done !


        //fire and forget:
        std::async(std::launch::async, []{
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "second thread" << std::endl;}
        );//block: a future that is created by std::async waits on its dtor until its promise is done !

        std::cout << "main thread" << std::endl;

        std::cout << std::endl;

    }
}
#endif //MODERNCPPCONCURR_FIREANDFORGETFUTURES_H
