#include "TokenBucketLimiter.h"

TokenBucketLimiter::TokenBucketLimiter(double capacity, double refillRatePerSecond)
    : capacity(capacity), refillRate(refillRatePerSecond) {}

bool TokenBucketLimiter::allow(const std::string& clientKey) {
    std::lock_guard<std::mutex> lock(mtx);
    auto now = std::chrono::steady_clock::now();

    auto it = buckets.find(clientKey);
    if (it == buckets.end()) {
        buckets[clientKey] = Bucket{capacity - 1, now};
        return true;
    }

    Bucket& bucket = it->second;
    double elapsed = std::chrono::duration<double>(now - bucket.lastRefill).count();
    bucket.tokens = std::min(capacity, bucket.tokens + elapsed * refillRate);
    bucket.lastRefill = now;

    if (bucket.tokens >= 1.0) {
        bucket.tokens -= 1.0;
        return true;
    }
    return false;
}
