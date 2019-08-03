#ifndef MODERNCPPCONCURR_SINGLETON_H
#define MODERNCPPCONCURR_SINGLETON_H
#include <chrono>
#include <iostream>
#include <future>
#include <mutex>
#include <thread>
#include <vector>
namespace Singleton
{
    //1. Double Checked Locking Pattern
    class Singleton_DoubleCheckLocking
    {
        static std::unique_ptr<Singleton_DoubleCheckLocking> m_instance;
        static std::mutex m_mutex;
        Singleton_DoubleCheckLocking(){ std::cout << "Singleton_DoubleCheckLocking::Ctor" << std::endl; }
        Singleton_DoubleCheckLocking(const Singleton_DoubleCheckLocking &) = delete;
        Singleton_DoubleCheckLocking(const Singleton_DoubleCheckLocking &&) = delete;
        Singleton_DoubleCheckLocking & operator=(const Singleton_DoubleCheckLocking &) = delete;
        Singleton_DoubleCheckLocking & operator=(const Singleton_DoubleCheckLocking &&) = delete;
    public:
        static Singleton_DoubleCheckLocking & Instance()
        {
            if (!m_instance)
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (!m_instance)
                    m_instance.reset(new Singleton_DoubleCheckLocking);
                /*
                 * Double Checked Locking Pattern is not thread safe: (dynamic memory allocation is not thread safe in a critical section)
                 * m_instance = new Singleton() consists of 3 steps
                 * 1. allocate memory for Singleton
                 * 2. initialize Singleton object
                 * 3. let m_instance point to the fully initialized Singleton object
                 * These steps might be reordered to 1,3,2.
                 * Another thread might access to m_instance when it points to an non-initialized object between step 3 and 2.
                 */
            }
            return *m_instance; //equivalent to *m_instance.get()
        }
        ~Singleton_DoubleCheckLocking(){ std::cout << "Singleton_DoubleCheckLocking::Dtor" << std::endl; }
    };
    std::unique_ptr<Singleton_DoubleCheckLocking> Singleton_DoubleCheckLocking::m_instance;
    std::mutex Singleton_DoubleCheckLocking::m_mutex;

    //2. Meyers Singleton
    class Singleton_Meyers
    {
        Singleton_Meyers() { std::cout << "Singleton_Meyers::Ctor" << std::endl; }
        Singleton_Meyers(const Singleton_Meyers &) = delete;
        Singleton_Meyers(const Singleton_Meyers &&) = delete;
        Singleton_Meyers & operator=(const Singleton_Meyers &) = delete;
        Singleton_Meyers & operator=(const Singleton_Meyers &&) = delete;
    public:
        static Singleton_Meyers & Instance()
        {
            //C++11 guarantees that static variables with block scope will be initialized in a thread-safe way
           static Singleton_Meyers m_instance;
            volatile int PreventCompilerFromOptimizeAwayCallingThisMethod{};
            return m_instance;
        }
        ~Singleton_Meyers(){ std::cout << "Singleton_Meyers::Dtor" << std::endl; }
    };

    //3. once_flag & call_once
    class Singleton_OnceFlag
    {
        //static Singleton_OnceFlag * m_instance;
        static std::unique_ptr<Singleton_OnceFlag> m_instance;
        static std::once_flag m_flag;
        Singleton_OnceFlag() { std::cout << "Singleton_OnceFlag::Ctor" << std::endl; }
        //Singleton_OnceFlag() = default;
        Singleton_OnceFlag(const Singleton_OnceFlag &) = delete;
        Singleton_OnceFlag(const Singleton_OnceFlag &&) = delete;
        Singleton_OnceFlag & operator=(const Singleton_OnceFlag &) = delete;
        Singleton_OnceFlag & operator=(const Singleton_OnceFlag &&) = delete;
    public:
        static Singleton_OnceFlag & Instance()
        {
            std::call_once(m_flag, []
            {
                std::cout << "Singleton_OnceFlag::Init" << std::endl;
                //m_instance = new Singleton_OnceFlag; //calling ctor here for once !
                m_instance.reset(new Singleton_OnceFlag); //calling ctor here for once !
            });
            volatile int PreventCompilerFromOptimizeAwayCallingThisMethod{};
            //return *m_instance;
            return *m_instance;
        }
        ~Singleton_OnceFlag() { std::cout << "Singleton_OnceFlag::Dtor" << std::endl; }
        //~Singleton_OnceFlag() = default;
    };
    //Singleton_OnceFlag * Singleton_OnceFlag::m_instance = nullptr;
    std::unique_ptr<Singleton_OnceFlag> Singleton_OnceFlag::m_instance;
    std::once_flag Singleton_OnceFlag::m_flag;

    //4. atomic sequential consistency: fix the issue from Double Checked Locking Pattern
    class Singleton_SeqCst
    {
        static std::atomic<Singleton_SeqCst*> m_instance;
        static std::mutex m_mutex;
        Singleton_SeqCst() { std::cout << "Singleton_SeqCst::Ctor" << std::endl; }
        Singleton_SeqCst(const Singleton_SeqCst &) = delete;
        Singleton_SeqCst(const Singleton_SeqCst &&) = delete;
        Singleton_SeqCst & operator=(const Singleton_SeqCst &) = delete;
        Singleton_SeqCst & operator=(const Singleton_SeqCst &&) = delete;
    public:
        static Singleton_SeqCst & Instance()
        {
            Singleton_SeqCst * res = m_instance.load();
            if (!res)
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                res = m_instance.load(std::memory_order_relaxed);
                if (!res)
                {
                    res = new Singleton_SeqCst;
                    m_instance.store(res);//now guarantees the dynamic memory allocation happen before the store operation
                }
            }
            volatile int PreventCompilerFromOptimizeAwayCallingThisMethod{};
            return *res;
        }
        ~Singleton_SeqCst(){ std::cout << "Singleton_SeqCst::Dtor" << std::endl; }
    };
    std::atomic<Singleton_SeqCst*> Singleton_SeqCst::m_instance;
    std::mutex Singleton_SeqCst::m_mutex;

    //5. atomic acquire release: fix the issue from Double Checked Locking Pattern
    class Singleton_AcqRel
    {
        static std::atomic<Singleton_AcqRel*> m_instance;
        static std::mutex m_mutex;
        Singleton_AcqRel() { std::cout << "Singleton_AcqRel::Ctor" << std::endl; }
        Singleton_AcqRel(const Singleton_AcqRel &) = delete;
        Singleton_AcqRel(const Singleton_AcqRel &&) = delete;
        Singleton_AcqRel & operator=(const Singleton_AcqRel &) = delete;
        Singleton_AcqRel & operator=(const Singleton_AcqRel &&) = delete;
    public:
        static Singleton_AcqRel & Instance()
        {
            Singleton_AcqRel * res = m_instance.load(std::memory_order_acquire);
            if (!res)
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                res = m_instance.load(std::memory_order_relaxed);
                if (!res)
                {
                    res = new Singleton_AcqRel;
                    m_instance.store(res, std::memory_order_release);//now guarantees the dynamic memory allocation happen before the store operation
                }
            }
            volatile int PreventCompilerFromOptimizeAwayCallingThisMethod{};
            return *res;
        }
        ~Singleton_AcqRel(){ std::cout << "Singleton_AcqRel::Dtor" << std::endl; }
    };
    std::atomic<Singleton_AcqRel*> Singleton_AcqRel::m_instance;
    std::mutex Singleton_AcqRel::m_mutex;


    void Run()
    {
        {
            const auto begin = std::chrono::system_clock::now();
            std::vector<std::thread> v;
            for (int i = 0; i < 4000; ++i)
                v.emplace_back(Singleton_DoubleCheckLocking::Instance);
            for (int i = 0; i < 4000; ++i)
                v[i].join();
            const auto end = std::chrono::system_clock::now() - begin;
            std::cout << "Singleton_DoubleCheckLocking: " << std::chrono::duration<double>(end).count() << std::endl;
            std::vector<std::thread>().swap(v);
        }
        {
            const auto begin = std::chrono::system_clock::now();
            std::vector<std::thread> v;
            for (int i = 0; i < 4000; ++i)
                v.emplace_back(Singleton_Meyers::Instance);
            for (int i = 0; i < 4000; ++i)
                v[i].join();
            const auto end = std::chrono::system_clock::now() - begin;
            std::cout << "Singleton_Meyers: " << std::chrono::duration<double>(end).count() << std::endl;
            std::vector<std::thread>().swap(v);
        }
        {
            const auto begin = std::chrono::system_clock::now();
            std::vector<std::thread> v;
            for (int i = 0; i < 4000; ++i)
                v.emplace_back(Singleton_OnceFlag::Instance);
            for (int i = 0; i < 4000; ++i)
                v[i].join();
            const auto end = std::chrono::system_clock::now() - begin;
            std::cout << "Singleton_OnceFlag: " << std::chrono::duration<double>(end).count() << std::endl;
        }
        {
            const auto begin = std::chrono::system_clock::now();
            std::vector<std::thread> v;
            for (int i = 0; i < 4000; ++i)
                v.emplace_back(Singleton_SeqCst::Instance);
            for (int i = 0; i < 4000; ++i)
                v[i].join();
            const auto end = std::chrono::system_clock::now() - begin;
            std::cout << "Singleton_SeqCst: " << std::chrono::duration<double>(end).count() << std::endl;
        }
        {
            const auto begin = std::chrono::system_clock::now();
            std::vector<std::thread> v;
            for (int i = 0; i < 4000; ++i)
                v.emplace_back(Singleton_AcqRel::Instance);
            for (int i = 0; i < 4000; ++i)
                v[i].join();
            const auto end = std::chrono::system_clock::now() - begin;
            std::cout << "Singleton_AcqRel: " << std::chrono::duration<double>(end).count() << std::endl;
        }
    }
}
#endif //MODERNCPPCONCURR_SINGLETON_H
