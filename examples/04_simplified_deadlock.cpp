#include <chrono>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <map>
#include <syncstream>
#include <algorithm>

using namespace std::chrono_literals;

struct Employee
{
    std::map<std::string, uint64_t> lunch_partners_counter;
    std::string id;
    std::mutex m;

    Employee(std::string id) : id(id) {}

    std::string partners() const
    {
        std::string ret = "Employee " + id + " has lunch partners: ";
        for (int count{}; const auto& partner : lunch_partners_counter)
            ret += (count++ ? ", " : "") + partner.first + " (" + std::to_string(partner.second) + ')';
        return ret;
    }
};

void assign_lunch_partners(Employee& first, Employee& second, Employee& third)
{
    // Print waiting message for all partners
    std::osyncstream(std::cout) << first.id << " and " << second.id << " and " << third.id << " are waiting for locks.\n";

    // Lock all mutexes at once to avoid deadlock
    std::scoped_lock lock(first.m, second.m, third.m);

    // Print got locks message for all partners
    std::osyncstream(std::cout) << first.id << " and " << second.id << " and " << third.id << " got locks.\n";

    // Update the lunch counters for each pair of employees
    ++first.lunch_partners_counter[second.id];
    ++first.lunch_partners_counter[third.id];
    ++second.lunch_partners_counter[first.id];
    ++second.lunch_partners_counter[third.id];
    ++third.lunch_partners_counter[first.id];
    ++third.lunch_partners_counter[second.id];

    // Wait a while
    std::this_thread::sleep_for(100ms);

    // Messages after releasing the locks
    std::osyncstream(std::cout) << first.id << " and " << second.id << " and " << third.id << " have released their locks.\n";
}

void assign_lunch_partners_deadlock(Employee& first, Employee& second, Employee& third)
{
    // Print waiting message for all partners
    std::osyncstream(std::cout) << first.id << " and " << second.id << " and " << third.id << " are waiting for locks.\n";

    // Lock all mutexes but deadlock can occur
    first.m.lock();
    second.m.lock();
    third.m.lock();

    // Print got locks message for all partners
    std::osyncstream(std::cout) << first.id << " and " << second.id << " and " << third.id << " got locks.\n";

    // Update the lunch counters for each pair of employees
    ++first.lunch_partners_counter[second.id];
    ++first.lunch_partners_counter[third.id];
    ++second.lunch_partners_counter[first.id];
    ++second.lunch_partners_counter[third.id];
    ++third.lunch_partners_counter[first.id];
    ++third.lunch_partners_counter[second.id];

    // Wait a while
    std::this_thread::sleep_for(100ms);

    // Messages after releasing the locks
    std::osyncstream(std::cout) << first.id << " and " << second.id << " and " << third.id << " have released their locks.\n";
}

int main()
{
    std::map<int, Employee> employees;

    employees.emplace(0, "A");
    employees.emplace(1, "B");
    employees.emplace(2, "C");

    {
        std::vector<std::jthread> threads;
        std::vector<int> set = {0, 1, 2};
        std::sort(set.begin(), set.end());

        do {
            threads.emplace_back([set, &employees](){
                assign_lunch_partners(employees.at(set[0]), employees.at(set[1]),
                                      employees.at(set[2]));});
//            threads.emplace_back([set, &employees](){
//                assign_lunch_partners_deadlock(employees.at(set[0]), employees.at(set[1]),
//                                      employees.at(set[2]));});
        } while (std::next_permutation(set.begin(), set.end()));
    }

    for(auto &employee : employees)
    {
        std::cout << employee.second.partners() << '\n';
    }
}