#include "Logger.h"
#include <iostream>
namespace tuga4d {
    void Logger::Print(const char* sev, const std::string& msg) {
        std::cout << sev << ' ' << msg << "\n";
    }
}