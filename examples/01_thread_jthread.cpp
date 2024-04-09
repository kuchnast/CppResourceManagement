#include <chrono>
#include <thread>
#include <iostream>
#include <syncstream>
#include <sstream>

using namespace std::chrono_literals;

void thread_without_join()
{
    std::thread thread{[] (){
        while (true){
            std::cout << "Doing work\n";
            std::this_thread::sleep_for(1s);
        }
    }};
    std::this_thread::sleep_for(5s);
}

void jthread_without_join ()
{
    std::jthread thread{[] (){
        while (true){
            std::cout << "Doing work\n";
            std::this_thread::sleep_for(1s);
        }
    }};
    std::this_thread::sleep_for(5s);
}

void jthread_with_stop_and_join()
{
    std::jthread thread{[] {
        while (true){
            std::cout << "Doing work\n";
            std::this_thread::sleep_for(1s);
        }
    }};
    std::this_thread::sleep_for(5s);
    thread.request_stop();
    thread.join();
}

void jthread_with_stop_token()
{
    std::jthread thread{[] (const std::stop_token& st) {
        while (!st.stop_requested()) {
            std::cout << "Doing work\n";
            std::this_thread::sleep_for(1s);
        }
    }};

    std::this_thread::sleep_for(5s);
    thread.request_stop();
}

void jthread_with_stop_callback()
{
    std::jthread thread{[] (const std::stop_token& st) {
        while (!st.stop_requested()) {
            std::cout << "Doing work\n";
            std::this_thread::sleep_for(1s);
        }
    }};

    std::stop_callback callback(thread.get_stop_token(), []{
        std::cout << "Thread requested to stop\n";
    });

    std::this_thread::sleep_for(5s);
    thread.request_stop();
}

void jthread_with_stop_source_using_cout()
{
    std::vector<std::jthread> threads;
    std::stop_source stop_source;

    for (int i = 0; i < 4; ++i){
        threads.emplace_back( [] (int id, const std::stop_source& stop_source) {
            std::stop_token st = stop_source.get_token();
            while (!st.stop_requested()) {
                std::cout << "Thread " << id << " doing work\n";
                std::this_thread::sleep_for(1s);
            }
        }, i, stop_source);
    }

    std::this_thread::sleep_for(5s);
    std::cout << "Request stop from main thread\n";
    stop_source.request_stop();
}

void jthread_with_stop_source_using_fixed_cout()
{
    std::vector<std::jthread> threads;
    std::stop_source stop_source;

    for (int i = 0; i < 4; ++i){
        threads.emplace_back( [] (int id, const std::stop_source& stop_source) {
            std::stop_token st = stop_source.get_token();
            while (!st.stop_requested()) {
                std::stringstream ss;
                ss << "Thread " << id << " doing work\n";
                std::cout << ss.str();
                std::this_thread::sleep_for(1s);
            }
        }, i, stop_source);
    }

    std::this_thread::sleep_for(5s);
    std::cout << "Request stop from main thread\n";
    stop_source.request_stop();
}

void jthread_with_stop_source_using_printf()
{
    std::vector<std::jthread> threads;
    std::stop_source stop_source;

    for (int i = 0; i < 4; ++i){
        threads.emplace_back([] (int id, const std::stop_source& stop_source){
                std::stop_token st = stop_source.get_token();
                while (!st.stop_requested()) {
                    std::printf("Thread %d doing work\n", id);
                    std::this_thread::sleep_for(1s);
                }
            }, i, stop_source);
    }

    std::this_thread::sleep_for(5s);
    std::cout << "Request stop from main thread\n";
    stop_source.request_stop();
}

void jthread_with_stop_source_using_osyncstream()
{
    std::vector<std::jthread> threads;
    std::stop_source stop_source;

    for (int i = 0; i < 4; ++i){
        threads.emplace_back( [] (int id, const std::stop_source& stop_source) {
            std::stop_token st = stop_source.get_token();
            while (!st.stop_requested()) {
                std::osyncstream(std::cout) << "Thread " << id << " doing work\n";
                std::this_thread::sleep_for(1s);
            }
        }, i, stop_source);
    }

    std::this_thread::sleep_for(5s);
    std::cout << "Request stop from main thread\n";
    stop_source.request_stop();
}

int main()
{
//    thread_without_join();
//    jthread_without_join();
//
//    jthread_with_stop_and_join();
//    jthread_with_stop_token();
//    jthread_with_stop_callback();
//
//    jthread_with_stop_source_using_cout();
    jthread_with_stop_source_using_fixed_cout();
//    jthread_with_stop_source_using_printf();
//    jthread_with_stop_source_using_osyncstream();
}