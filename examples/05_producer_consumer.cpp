#include <iostream>
#include <thread>
#include <array>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <algorithm>

std::mutex m;
std::condition_variable_any producer_cv, consumer_cv;
std::queue<int> buffer;

constexpr int buffer_size = 10;

void consumer_thread(int id, std::stop_source stop_source)
{
    std::stop_token st = stop_source.get_token();
    while (!st.stop_requested())
    {
        std::unique_lock<std::mutex> lk(m);

        if (!buffer.empty()) {
            std::cout << "Consumer " << id << " ate " << buffer.front() << std::endl;
            buffer.pop();
            producer_cv.notify_all();
        } else {
            std::cout << "*** Consumer " << id << " is waiting" << std::endl;
            consumer_cv.wait(lk, [&st] { return (!buffer.empty()) || st.stop_requested(); });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 400 + 800));
    }
}

void producer_thread(int id, std::stop_source stop_source)
{
    std::stop_token st = stop_source.get_token();
    while (!st.stop_requested())
    {
        std::unique_lock<std::mutex> lk(m);

        if (buffer.size() < buffer_size)
        {
            int a = rand() % 400;
            std::cout << "Producer " << id << " produced: " << a << std::endl;

            buffer.push(a);
            consumer_cv.notify_all();
        }
        else
        {
            std::cout << "### Producer " << id << " is waiting" << std::endl;
            producer_cv.wait(lk, []{ return buffer.size() < buffer_size; });
        }

        lk.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 400 + 100));
    }
}

int main()
{
    {
        const int consumers_count = 3;
        const int producers_count = 3;

        std::vector<std::jthread> producers;
        std::vector<std::jthread> consumers;
        std::stop_source p_stop_source, c_stop_source;

        for (int i = 0; i < consumers_count; i++)
            consumers.emplace_back(consumer_thread, i + 1, c_stop_source);
        for (int i = 0; i < producers_count; i++)
            producers.emplace_back(producer_thread, i + 1, p_stop_source);

        while (std::cin.get() != '\n'){};

        p_stop_source.request_stop();
        for (auto& producer : producers)
            producer.join();

        {
            std::unique_lock<std::mutex> lk(m);
            consumer_cv.wait(lk, []{ return buffer.empty(); });
        }

        c_stop_source.request_stop();
        consumer_cv.notify_all();
    }

    std::cout << "\nElements in buffer: " << buffer.size() << std::endl;

    return 0;
}