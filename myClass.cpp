#ifndef MYCLASS_H
#define MYCLASS_H
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>

const std::string LOG_FILE = "myclass.log";

void logMessage(const std::string& message) {
    std::ofstream logFile(LOG_FILE, std::ios_base::app);
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);

    logFile << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] " << message << std::endl;
}

class MyClass {
public:
    int id;
    char name[50];

    MyClass(int id, const std::string& name) {
        this->id = id;
        std::strncpy(this->name, name.c_str(), sizeof(this->name) - 1);
        this->name[sizeof(this->name) - 1] = '\0';
        logMessage("MyClass created with id=" + std::to_string(id) + " and name=" + this->name);
    }

    MyClass() : id(0) {
        std::memset(name, 0, sizeof(name));
        logMessage("Default MyClass created");
    }
};

#endif
