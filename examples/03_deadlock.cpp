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

template<typename... EmployeesNames>
void print_progress(const std::string &msg, const std::string& first_id, EmployeesNames&&... partners_id)
{
    std::stringstream ss;
    ss << first_id;
    ((ss << " and " << std::forward<EmployeesNames>(partners_id)), ...);
    ss << msg << std::endl;
    std::cout << ss.str();
}

template<typename... Employees>
void assign_lunch_partners(Employee& first, Employees&... partners)
{
    // Print waiting message for all partners.
    print_progress(" are waiting for locks.", first.id, std::forward<Employees>(partners).id...);

    // Lock all mutexes at once to avoid deadlock.
    std::scoped_lock lock(first.m, partners.m...);

    // Print got locks message for all partners.
    print_progress(" got locks.", first.id, std::forward<Employees>(partners).id...);

    // Update the lunch counters for each pair of employees.
    ((++first.lunch_partners_counter[partners.id] && ++partners.lunch_partners_counter[first.id]), ...);
    std::vector<std::reference_wrapper<Employee>> emp_list = {partners...};
    for (size_t i = 0; i < emp_list.size(); ++i) {
        for (size_t j = i + 1; j < emp_list.size(); ++j) {
            Employee& f = emp_list[i];
            Employee& s = emp_list[j];

            ++f.lunch_partners_counter[s.id];
            ++s.lunch_partners_counter[f.id];
        }
    }

    // Wait a while
    std::this_thread::sleep_for(100ms);

    // Messages after releasing the locks
    print_progress(" have released their locks.", first.id, std::forward<Employees>(partners).id...);
}

template<typename... Employees>
void assign_lunch_partners_deadlock(Employee& first, Employees&... partners)
{
    // Print waiting message for all partners.
    print_progress(" are waiting for locks.", first.id, std::forward<Employees>(partners).id...);

    // Lock all mutexes at once to avoid deadlock.
    first.m.lock();
    (std::forward<Employees>(partners).m.lock(), ...);

    // Print got locks message for all partners.
    print_progress(" got locks.", first.id, std::forward<Employees>(partners).id...);

    // Update the lunch counters for each pair of employees.
    ((++first.lunch_partners_counter[partners.id] && ++partners.lunch_partners_counter[first.id]), ...);

    // Wait a while
    std::this_thread::sleep_for(100ms);

    // Messages after releasing the locks
    print_progress(" have released their locks.", first.id, std::forward<Employees>(partners).id...);
}

int main()
{
    std::map<int, Employee> employees;

    employees.emplace(0, "A");
    employees.emplace(1, "B");
    employees.emplace(2, "C");
    employees.emplace(3, "D");

    {
        std::vector<std::jthread> threads;
        std::vector<int> set = {0, 1, 2, 3};
        std::sort(set.begin(), set.end());

        do {
            threads.emplace_back([set, &employees](){
                assign_lunch_partners(employees.at(set[0]), employees.at(set[1]),
                                      employees.at(set[2]), employees.at(set[3]));});
//            threads.emplace_back([set, &employees](){
//                assign_lunch_partners_deadlock(employees.at(set[0]), employees.at(set[1]),
//                                               employees.at(set[2]), employees.at(set[3]));});
        } while (std::next_permutation(set.begin(), set.end()));
    }

    for(auto &employee : employees)
    {
        std::cout << employee.second.partners() << '\n';
    }
}