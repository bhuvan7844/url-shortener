#include <gtest/gtest.h>
#include "Encoder.h"
#include <set>

TEST(EncoderTest, RoundTripBoundaryValues) {
    Encoder encoder;
    std::vector<int64_t> ids = {0, 1, 61, 62, 1000, 1000000};
    for (auto id : ids) {
        std::string code = encoder.encode(id);
        EXPECT_EQ(encoder.decode(code), id);
    }
}

TEST(EncoderTest, NoCollisionsInRange) {
    Encoder encoder;
    std::set<std::string> seen;
    for (int64_t id = 1; id <= 100000; id++) {
        std::string code = encoder.encode(id);
        EXPECT_TRUE(seen.find(code) == seen.end()) << "Collision at id=" << id;
        seen.insert(code);
    }
}

TEST(EncoderTest, DecodeInvalidCharacterThrows) {
    Encoder encoder;
    EXPECT_THROW(encoder.decode("AB!C"), std::invalid_argument);
}
