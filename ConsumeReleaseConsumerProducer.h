#ifndef EDUCATIVEMODERNCPPCONCUR_CONSUMERELEASECONSUMERPRODUCER_H
#define EDUCATIVEMODERNCPPCONCUR_CONSUMERELEASECONSUMERPRODUCER_H
#include <atomic>
#include <thread>
#include <iostream>
#include <string>
namespace ConsumeReleaseConsumerProducer
{
    using namespace std;

    atomic<string*> ptr;
    int data;
    atomic<int> atoData;

    void producer()
    {
        string* p  = new string("C++11");
        data = 2011; // data race !!!
        atoData.store(2014, memory_order_relaxed);
        ptr.store(p, memory_order_release);
    }

    void consumer()
    {
        string* p2;
        while (!(p2 = ptr.load(memory_order_consume)));
        cout << "*p2: " << *p2 << endl;
        cout << "data: " << data << endl; // data race !!!
        cout << "atoData: " << atoData.load(memory_order_relaxed) << endl; // can be either 2014 or
    }

    void Run()
    {

        cout << endl;

        thread t1(producer);
        thread t2(consumer);

        t1.join();
        t2.join();

        cout << endl;

    }
}
#endif
