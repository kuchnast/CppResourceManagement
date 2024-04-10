cmake_minimum_required(VERSION 3.26)
project(ZarzadzanieZasobami)

set(CMAKE_CXX_STANDARD 20)

add_executable(test test.cpp)

add_executable(01_thread_jthread examples/01_thread_jthread.cpp)
add_executable(02_false_sharing examples/02_false_sharing.cpp)
add_executable(03_deadlock examples/03_deadlock.cpp)
add_executable(04_simplified_deadlock examples/04_simplified_deadlock.cpp)
add_executable(05_producer_consumer examples/05_producer_consumer.cpp)
