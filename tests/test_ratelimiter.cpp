#include <gtest/gtest.h>
#include "TokenBucketLimiter.h"
#include <thread>
#include <chrono>

TEST(RateLimiterTest, AllowsRequestsWithinBurstCapacity) {
    TokenBucketLimiter limiter(3, 1.0);
    EXPECT_TRUE(limiter.allow("client1"));
    EXPECT_TRUE(limiter.allow("client1"));
    EXPECT_TRUE(limiter.allow("client1"));
}

TEST(RateLimiterTest, RejectsRequestAfterBurstExhausted) {
    TokenBucketLimiter limiter(2, 1.0);
    EXPECT_TRUE(limiter.allow("client2"));
    EXPECT_TRUE(limiter.allow("client2"));
    EXPECT_FALSE(limiter.allow("client2"));
}

TEST(RateLimiterTest, RefillsAfterWaiting) {
    TokenBucketLimiter limiter(1, 10.0); // refills 10 tokens/sec
    EXPECT_TRUE(limiter.allow("client3"));
    EXPECT_FALSE(limiter.allow("client3"));

    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // ~2 tokens worth
    EXPECT_TRUE(limiter.allow("client3"));
}

TEST(RateLimiterTest, DifferentClientsHaveSeparateBuckets) {
    TokenBucketLimiter limiter(1, 1.0);
    EXPECT_TRUE(limiter.allow("clientA"));
    EXPECT_TRUE(limiter.allow("clientB")); // separate bucket, unaffected by clientA
}
