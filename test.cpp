#include <chrono>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <map>
#include <syncstream>
#include <algorithm>
#include <vector>

using namespace std::chrono_literals;

struct Employee {
    std::map<std::string, uint64_t> lunch_partners_counter;
    std::string id;
    std::mutex m;

    Employee(std::string id) : id(std::move(id)) {}

    std::string partners() const {
        std::string ret = "Employee " + id + " has lunch partners: ";
        for (auto [partner, count] : lunch_partners_counter) {
            ret += partner + " (" + std::to_string(count) + "), ";
        }
        if (!lunch_partners_counter.empty()) {
            ret.pop_back(); // Remove the last space and comma
            ret.pop_back();
        }
        return ret;
    }
};

void send_mail(Employee&, Employee&) {
    std::this_thread::sleep_for(100ms);
}

void print_progress(const std::string& msg, const std::vector<std::reference_wrapper<Employee>>& employees) {
    std::stringstream ss;
    for (auto& emp : employees) {
        ss << emp.get().id << " ";
    }
    ss << msg << std::endl;
    std::cout << ss.str();
}

void assign_lunch_partners(Employee& employee1, Employee& employee2, Employee& employee3) {
    // Print waiting message for all partners.
    print_progress("are waiting for locks.", employees);

    std::vector<std::unique_lock<std::mutex>> locks;
    for (auto& emp : employees) {
        locks.emplace_back(emp.get().m, std::defer_lock);
    }
    std::lock(locks.begin(), locks.end());

    // Print got locks message for all partners.
    print_progress("got locks.", employees);

    // Update the lunch counters for each pair of employees.
    for (size_t i = 0; i < employees.size(); ++i) {
        for (size_t j = i + 1; j < employees.size(); ++j) {
            Employee& first = employees[i];
            Employee& second = employees[j];

            ++first.lunch_partners_counter[second.id];
            ++second.lunch_partners_counter[first.id];
        }
    }

    // Messages after releasing the locks
    print_progress("have released their locks.", employees);
}

int main() {
    std::map<int, Employee> employees;

    employees.emplace(0, "A");
    employees.emplace(1, "B");
    employees.emplace(2, "C");
    employees.emplace(3, "D");

    std::vector<std::reference_wrapper<Employee>> emp_refs;
    for (auto& [_, emp] : employees) {
        emp_refs.push_back(emp);
    }

    // Here you can use assign_lunch_partners with emp_refs directly
    // For example:
    assign_lunch_partners(emp_refs);

    for (auto& [_, employee] : employees) {
        std::cout << employee.partners() << '\n';
    }
}
