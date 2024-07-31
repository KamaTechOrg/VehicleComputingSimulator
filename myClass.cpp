#ifndef MYCLASS_H
#define MYCLASS_H
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>

class MyClass {
public:
    int id;
    char name[50];

    MyClass(int id, const std::string& name) {
        this->id = id;
        std::strncpy(this->name, name.c_str(), sizeof(this->name) - 1);
        this->name[sizeof(this->name) - 1] = '\0';
    }

    MyClass() : id(0) {
        std::memset(name, 0, sizeof(name));
    }
};

#endif

