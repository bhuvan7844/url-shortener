#pragma once
#include <string>
#include <chrono>

struct URLEntry {
    std::string shortCode;
    std::string longURL;
    int clicks = 0;
    std::chrono::system_clock::time_point createdAt;
};