#include <iostream>
#include <mutex>
#include <thread>
#include <array>

class Singleton {
private:
    static Singleton* instance;

protected:
    Singleton()
    {
        std::cout << "New instance of singleton created." << std::endl;
    }

public:
    Singleton(Singleton &other) = delete;
    void operator=(const Singleton &) = delete;

    static Singleton* getInstance() {
        if (instance == nullptr)
        {
            instance = new Singleton();
        }
        return instance;
    }
};

class MultithreadSingleton
{
private:
    static MultithreadSingleton* instance;
    static std::mutex mutex;

protected:
    MultithreadSingleton()
    {
        std::cout << "New instance of singleton created." << std::endl;
    }

public:
    MultithreadSingleton(MultithreadSingleton &other) = delete;
    void operator=(const MultithreadSingleton &) = delete;

    static MultithreadSingleton* getInstance() {
        if (instance == nullptr)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (instance == nullptr)
            {
                instance = new MultithreadSingleton();
            }
        }
        return instance;
    }
};

Singleton* Singleton::instance{nullptr};
MultithreadSingleton* MultithreadSingleton::instance{nullptr};
std::mutex MultithreadSingleton::mutex;
std::mutex cm;


int main() {
    constexpr int num_threads = 10;
    std::array<std::thread, num_threads> thr;

    for(int i = 0; i < num_threads; ++i)
    {
        thr[i] = std::thread([](int thr_num){
            std::lock_guard<std::mutex> lock(cm);
            Singleton* s1 = Singleton::getInstance();
            std::cout << "Thread " << thr_num << " class addresss " << s1 << std::endl;
            }, i);
    }

    for(auto &t : thr)
    {
        t.join();
    }

    return 0;
}
