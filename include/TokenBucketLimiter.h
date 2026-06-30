#pragma once
#include "RateLimiter.h"
#include <unordered_map>
#include <mutex>
#include <chrono>

struct Bucket {
    double tokens;
    std::chrono::steady_clock::time_point lastRefill;
};

class TokenBucketLimiter : public RateLimiter {
public:
    TokenBucketLimiter(double capacity, double refillRatePerSecond);
    bool allow(const std::string& clientKey) override;

private:
    double capacity;
    double refillRate;
    std::unordered_map<std::string, Bucket> buckets;
    std::mutex mtx;
};
