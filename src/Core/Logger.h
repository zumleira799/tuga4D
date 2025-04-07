#pragma once
#include <cstdio>
#include <string>
#include <stdexcept>

namespace tuga4d {
    struct Logger {
        template<typename ... Args>
        static void Trace(const std::string& fmc, Args... args) {
            std::string message = FormatString(fmc, args...);
            Print("[trace]", message);
        }

        template<typename ... Args>
        static void Error(const std::string& fmc, Args... args) {
            std::string message = FormatString(fmc, args...);
            Print("[error]", message);
        }

        template<typename ... Args>
        static void Warning(const std::string& fmc, Args... args) {
            std::string message = FormatString(fmc, args...);
            Print("[warn]", message);
        }

        template<typename ... Args>
        static void Info(const std::string& fmc, Args... args) {
            std::string message = FormatString(fmc, args...);
            Print("[info]", message);
        }
    private:
        template<typename ... Args>
        static std::string FormatString(const std::string& fmc, Args... args) {
            int size_s = std::snprintf(nullptr, 0, fmc.c_str(), args ...) + 1; // Extra space for '\0'
            if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
            auto size = static_cast<size_t>(size_s);
            char* buf = new char[size];
            std::snprintf(buf, size, fmc.c_str(), args ...);
            std::string result = std::string(buf, buf + size - 1); // We don't want the '\0' inside
            delete[] buf;
            return result;
        }

        static void Print(const char* sev, const std::string& msg);
    };
}