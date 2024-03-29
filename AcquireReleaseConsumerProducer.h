#ifndef MODERNCPPCONCURR_ACQUIRERELEASECONSUMERPRODUCER_H
#define MODERNCPPCONCURR_ACQUIRERELEASECONSUMERPRODUCER_H
#include <atomic>
#include <thread>
#include <iostream>
#include <string>
namespace AcquireReleaseConsumerProducer
{
    using namespace std;

    atomic<string*> ptr;
    int data;
    atomic<int> atoData;

    void producer()
    {
        string* p  = new string("C++11");
        data = 2011;
        atoData.store(2014, memory_order_relaxed);
        ptr.store(p, memory_order_release);
    }

    void consumer()
    {
        string* p2;
        while (!(p2 = ptr.load(memory_order_acquire)));
        cout << "*p2: " << *p2 << endl;
        cout << "data: " << data << endl;
        cout << "atoData: " << atoData.load(memory_order_relaxed) << endl;
    }

    void Run(){

        cout << endl;

        thread t1(producer);
        thread t2(consumer);

        t1.join();
        t2.join();

        cout << endl;

    }
}
#endif //MODERNCPPCONCURR_ACQUIRERELEASECONSUMERPRODUCER_H
