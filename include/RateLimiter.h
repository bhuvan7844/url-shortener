#pragma once
#include <string>

class RateLimiter {
public:
    virtual ~RateLimiter() = default;
    virtual bool allow(const std::string& clientKey) = 0;
};
