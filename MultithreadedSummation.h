#ifndef EDUCATIVEMODERNCPPCONCUR_MULTITHREADEDSUMMATION_H
#define EDUCATIVEMODERNCPPCONCUR_MULTITHREADEDSUMMATION_H
#include <chrono>
#include <iostream>
#include <random>
#include <vector>
#include <numeric>
#include <atomic>
#include <future>
namespace MultithreadedSummation
{
    constexpr long long size = 100000000;
    std::vector<int> randValues;
    std::random_device seed;
    std::mt19937 engine(seed());
    std::uniform_int_distribution<> uniformDist(1, 10);
    std::mutex myMutex;

    void sumUp_Lock(unsigned long long & sum, const std::vector<int> & val,
               unsigned long long beg, unsigned long long end)
    {
        for (auto it = beg; it < end; ++it)
        {
            std::lock_guard<std::mutex> myLock(myMutex);
            sum += val[it];
        }
    }
    void sumUp_AtomicOperatorPlusEqual(std::atomic<unsigned long long> & sum, const std::vector<int> & val,
               unsigned long long beg, unsigned long long end)
    {
        for (auto it = beg; it < end; ++it)
            sum += val[it];//seq_cst
    }
    void sumUp_AtomicFetchAdd(std::atomic<unsigned long long> & sum, const std::vector<int> & val,
               unsigned long long beg, unsigned long long end)
    {
        for (auto it = beg; it < end; ++it)
            sum.fetch_add(val[it]);//seq_cst
    }
    void sumUp_AtomicFetchAddRelaxed(std::atomic<unsigned long long> & sum, const std::vector<int> & val,
               unsigned long long beg, unsigned long long end)
    {
        for (auto it = beg; it < end; ++it)
            sum.fetch_add(val[it], std::memory_order_relaxed);
    }
    void sumUp_LocalVar_Lock(unsigned long long & sum, const std::vector<int> & val,
               unsigned long long beg, unsigned long long end)
    {
        unsigned long long tmpSum{};//thread local
        for (auto i = beg; i < end; ++i)
            tmpSum += val[i];

        std::lock_guard<std::mutex> lockGuard(myMutex);
        sum += tmpSum;
    }
    void sumUp_LocalVar_AtomicOperatorPlusEqual(std::atomic<unsigned long long> & sum, const std::vector<int> & val,
               unsigned long long beg, unsigned long long end)
    {
        unsigned int long long tmpSum{};//thread local
        for (auto i = beg; i < end; ++i)
            tmpSum += val[i];

        sum += tmpSum;//seq_cst
    }
    void sumUp_LocalVar_AtomicFetchAddRelaxed(std::atomic<unsigned long long> & sum, const std::vector<int> & val,
               unsigned long long beg, unsigned long long end)
    {
        unsigned int long long tmpSum{};//thread local
        for (auto i = beg; i < end; ++i)
            tmpSum += val[i];

        sum.fetch_add(tmpSum, std::memory_order_relaxed);
    }

    thread_local unsigned long long threadLocalSum = 0;
    void sumUp_ThreadLocal_AtomicFetchAddRelaxed(std::atomic<unsigned long long> & sum, const std::vector<int> & val,
               unsigned long long beg, unsigned long long end)
    {
        for (auto i = beg; i < end; ++i)
            threadLocalSum += val[i];

        sum.fetch_add(threadLocalSum, std::memory_order_relaxed);
    }

    void sumUp_LocalVar_Promise(std::promise<unsigned long long> && prom, const std::vector<int> & val,
               unsigned long long beg, unsigned long long end)
    {
        unsigned long long sum={};
        for (auto i = beg; i < end; ++i)
            sum += val[i];

        prom.set_value(sum);
    }

    void Run()
    {
        randValues.reserve(size);
        for (long long i = 0 ; i < size ; ++i)
            randValues.push_back(uniformDist(engine));

        {
            const auto sta = std::chrono::steady_clock::now();

            unsigned long long sum = {};
            for (auto n: randValues) sum += n;

            const std::chrono::duration<double> dur = std::chrono::steady_clock::now() - sta;
            std::cout << "Time for SingleThread_Loop " << dur.count() << " seconds" << std::endl;
            std::cout << "Result: " << sum << std::endl;
        }
        {
            const auto sta = std::chrono::steady_clock::now();

            const unsigned long long sum = std::accumulate(randValues.begin(), randValues.end(), 0);

            const std::chrono::duration<double> dur = std::chrono::steady_clock::now() - sta;

            std::cout << "Time for SingleThread_Accumulate " << dur.count() << " seconds" << std::endl;
            std::cout << "Result: " << sum << std::endl;
        }
        {
            const auto sta = std::chrono::steady_clock::now();

            unsigned long long sum = {};
            for (auto i: randValues)
            {
                std::lock_guard<std::mutex> myLockGuard(myMutex);
                sum += i;
            }

            const std::chrono::duration<double> dur = std::chrono::steady_clock::now() - sta;

            std::cout << "Time for SingleThread_LoopLock " << dur.count() << " seconds" << std::endl;
            std::cout << "Result: " << sum << std::endl;
        }
        {
            std::atomic<unsigned long long> sum = {};
            std::cout << std::boolalpha << "sum.is_lock_free(): " << sum.is_lock_free() << std::endl;
            auto sta = std::chrono::steady_clock::now();
            for (auto i: randValues) sum += i;
            std::chrono::duration<double> dur = std::chrono::steady_clock::now() - sta;
            std::cout << "Time for SingleThread_LoopAtomic operator+= " << dur.count() << " seconds" << std::endl;
            std::cout << "Result: " << sum << std::endl;

            sum = 0;
            sta = std::chrono::steady_clock::now();
            for (auto i: randValues) sum.fetch_add(i);
            dur = std::chrono::steady_clock::now() - sta;
            std::cout << "Time for SingleThread_LoopAtomic fetch_add " << dur.count() << " seconds" << std::endl;
            std::cout << "Result: " << sum << std::endl;
        }

        constexpr long long fir =  25000000;
        constexpr long long sec =  50000000;
        constexpr long long thi =  75000000;
        constexpr long long fou = 100000000;
        {
            unsigned long long sum = 0;
            const auto sta = std::chrono::steady_clock::now();

            std::thread t1(sumUp_Lock, std::ref(sum), std::ref(randValues), 0, fir);
            std::thread t2(sumUp_Lock, std::ref(sum), std::ref(randValues), fir, sec);
            std::thread t3(sumUp_Lock, std::ref(sum), std::ref(randValues), sec, thi);
            std::thread t4(sumUp_Lock, std::ref(sum), std::ref(randValues), thi, fou);

            t1.join();
            t2.join();
            t3.join();
            t4.join();

            std::chrono::duration<double> dur= std::chrono::steady_clock::now() - sta;
            std::cout << "Time for Multithread_Lock " << dur.count() << " seconds" << std::endl;
            std::cout << "Result: " << sum << std::endl;
        }
        {
            std::atomic<unsigned long long> sum = 0;
            const auto sta = std::chrono::steady_clock::now();

            std::thread t1(sumUp_AtomicOperatorPlusEqual, std::ref(sum), std::ref(randValues), 0, fir);
            std::thread t2(sumUp_AtomicOperatorPlusEqual, std::ref(sum), std::ref(randValues), fir, sec);
            std::thread t3(sumUp_AtomicOperatorPlusEqual, std::ref(sum), std::ref(randValues), sec, thi);
            std::thread t4(sumUp_AtomicOperatorPlusEqual, std::ref(sum), std::ref(randValues), thi, fou);

            t1.join();
            t2.join();
            t3.join();
            t4.join();

            std::chrono::duration<double> dur= std::chrono::steady_clock::now() - sta;
            std::cout << "Time for Multithread_AtomicOperatorPlusEqual " << dur.count() << " seconds" << std::endl;
            std::cout << "Result: " << sum << std::endl;
        }
        {
            std::atomic<unsigned long long> sum = 0;
            const auto sta = std::chrono::steady_clock::now();

            std::thread t1(sumUp_AtomicFetchAdd, std::ref(sum), std::ref(randValues), 0, fir);
            std::thread t2(sumUp_AtomicFetchAdd, std::ref(sum), std::ref(randValues), fir, sec);
            std::thread t3(sumUp_AtomicFetchAdd, std::ref(sum), std::ref(randValues), sec, thi);
            std::thread t4(sumUp_AtomicFetchAdd, std::ref(sum), std::ref(randValues), thi, fou);

            t1.join();
            t2.join();
            t3.join();
            t4.join();

            std::chrono::duration<double> dur= std::chrono::steady_clock::now() - sta;
            std::cout << "Time for Multithread_AtomicFetchAdd " << dur.count() << " seconds" << std::endl;
            std::cout << "Result: " << sum << std::endl;
        }
        {
            std::atomic<unsigned long long> sum = 0;
            const auto sta = std::chrono::steady_clock::now();

            std::thread t1(sumUp_AtomicFetchAddRelaxed, std::ref(sum), std::ref(randValues), 0, fir);
            std::thread t2(sumUp_AtomicFetchAddRelaxed, std::ref(sum), std::ref(randValues), fir, sec);
            std::thread t3(sumUp_AtomicFetchAddRelaxed, std::ref(sum), std::ref(randValues), sec, thi);
            std::thread t4(sumUp_AtomicFetchAddRelaxed, std::ref(sum), std::ref(randValues), thi, fou);

            t1.join();
            t2.join();
            t3.join();
            t4.join();

            std::chrono::duration<double> dur= std::chrono::steady_clock::now() - sta;
            std::cout << "Time for Multithread_AtomicFetchAddRelaxed " << dur.count() << " seconds" << std::endl;
            std::cout << "Result: " << sum << std::endl;
        }
        {
            unsigned long long sum = 0;
            const auto sta = std::chrono::steady_clock::now();

            std::thread t1(sumUp_LocalVar_Lock, std::ref(sum), std::ref(randValues), 0, fir);
            std::thread t2(sumUp_LocalVar_Lock, std::ref(sum), std::ref(randValues), fir, sec);
            std::thread t3(sumUp_LocalVar_Lock, std::ref(sum), std::ref(randValues), sec, thi);
            std::thread t4(sumUp_LocalVar_Lock, std::ref(sum), std::ref(randValues), thi, fou);

            t1.join();
            t2.join();
            t3.join();
            t4.join();

            std::chrono::duration<double> dur= std::chrono::steady_clock::now() - sta;
            std::cout << "Time for Multithread_LocalVar_Lock " << dur.count() << " seconds" << std::endl;
            std::cout << "Result: " << sum << std::endl;
        }
        {
            std::atomic<unsigned long long> sum = 0;
            const auto sta = std::chrono::steady_clock::now();

            std::thread t1(sumUp_LocalVar_AtomicOperatorPlusEqual, std::ref(sum), std::ref(randValues), 0, fir);
            std::thread t2(sumUp_LocalVar_AtomicOperatorPlusEqual, std::ref(sum), std::ref(randValues), fir, sec);
            std::thread t3(sumUp_LocalVar_AtomicOperatorPlusEqual, std::ref(sum), std::ref(randValues), sec, thi);
            std::thread t4(sumUp_LocalVar_AtomicOperatorPlusEqual, std::ref(sum), std::ref(randValues), thi, fou);

            t1.join();
            t2.join();
            t3.join();
            t4.join();

            std::chrono::duration<double> dur= std::chrono::steady_clock::now() - sta;
            std::cout << "Time for Multithread_LocalVar_AtomicOperatorPlusEqual " << dur.count() << " seconds" << std::endl;
            std::cout << "Result: " << sum << std::endl;
        }
        {
            std::atomic<unsigned long long> sum = 0;
            const auto sta = std::chrono::steady_clock::now();

            std::thread t1(sumUp_LocalVar_AtomicFetchAddRelaxed, std::ref(sum), std::ref(randValues), 0, fir);
            std::thread t2(sumUp_LocalVar_AtomicFetchAddRelaxed, std::ref(sum), std::ref(randValues), fir, sec);
            std::thread t3(sumUp_LocalVar_AtomicFetchAddRelaxed, std::ref(sum), std::ref(randValues), sec, thi);
            std::thread t4(sumUp_LocalVar_AtomicFetchAddRelaxed, std::ref(sum), std::ref(randValues), thi, fou);

            t1.join();
            t2.join();
            t3.join();
            t4.join();

            std::chrono::duration<double> dur= std::chrono::steady_clock::now() - sta;
            std::cout << "Time for Multithread_LocalVar_AtomicFetchAddRelaxed " << dur.count() << " seconds" << std::endl;
            std::cout << "Result: " << sum << std::endl;
        }
        {
            std::atomic<unsigned long long> sum = 0;
            const auto sta = std::chrono::steady_clock::now();

            std::thread t1(sumUp_ThreadLocal_AtomicFetchAddRelaxed, std::ref(sum), std::ref(randValues), 0, fir);
            std::thread t2(sumUp_ThreadLocal_AtomicFetchAddRelaxed, std::ref(sum), std::ref(randValues), fir, sec);
            std::thread t3(sumUp_ThreadLocal_AtomicFetchAddRelaxed, std::ref(sum), std::ref(randValues), sec, thi);
            std::thread t4(sumUp_ThreadLocal_AtomicFetchAddRelaxed, std::ref(sum), std::ref(randValues), thi, fou);

            t1.join();
            t2.join();
            t3.join();
            t4.join();

            std::chrono::duration<double> dur= std::chrono::steady_clock::now() - sta;
            std::cout << "Time for Multithread_ThreadLocal_AtomicFetchAddRelaxed " << dur.count() << " seconds" << std::endl;
            std::cout << "Result: " << sum << std::endl;
        }
        {
            std::promise<unsigned long long> prom1;
            std::promise<unsigned long long> prom2;
            std::promise<unsigned long long> prom3;
            std::promise<unsigned long long> prom4;

            auto fut1 = prom1.get_future();
            auto fut2 = prom2.get_future();
            auto fut3 = prom3.get_future();
            auto fut4 = prom4.get_future();

            const auto sta = std::chrono::system_clock::now();

            std::thread t1(sumUp_LocalVar_Promise, std::move(prom1), std::ref(randValues), 0, fir);
            std::thread t2(sumUp_LocalVar_Promise, std::move(prom2), std::ref(randValues), fir, sec);
            std::thread t3(sumUp_LocalVar_Promise, std::move(prom3), std::ref(randValues), sec, thi);
            std::thread t4(sumUp_LocalVar_Promise, std::move(prom4), std::ref(randValues), thi, fou);

            auto sum = fut1.get() + fut2.get() + fut3.get() + fut4.get();

            std::chrono::duration<double> dur= std::chrono::system_clock::now() - sta;
            std::cout << "Time for Multithread_LocalVar_Promise " << dur.count() << " seconds" << std::endl;
            std::cout << "Result: " << sum << std::endl;

            t1.join();
            t2.join();
            t3.join();
            t4.join();
        }

    }
}
#endif //EDUCATIVEMODERNCPPCONCUR_MULTITHREADEDSUMMATION_H
