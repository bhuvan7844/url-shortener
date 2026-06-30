#include <gtest/gtest.h>
#include "InMemoryStorage.h"
#include <memory>

class StorageTest : public ::testing::Test {
protected:
    std::unique_ptr<Storage> storage = std::make_unique<InMemoryStorage>();
};

TEST_F(StorageTest, SaveAndGet) {
    URLEntry entry{"abc", "https://example.com", 0, std::chrono::system_clock::now()};
    EXPECT_TRUE(storage->save(entry));

    auto result = storage->get("abc");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->longURL, "https://example.com");
}

TEST_F(StorageTest, GetNonexistentReturnsNullopt) {
    auto result = storage->get("doesnotexist");
    EXPECT_FALSE(result.has_value());
}

TEST_F(StorageTest, IncrementClicksIncreasesCount) {
    URLEntry entry{"xyz", "https://test.com", 0, std::chrono::system_clock::now()};
    storage->save(entry);

    storage->incrementClicks("xyz");
    storage->incrementClicks("xyz");

    auto result = storage->get("xyz");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->clicks, 2);
}

TEST_F(StorageTest, IncrementClicksOnNonexistentReturnsFalse) {
    EXPECT_FALSE(storage->incrementClicks("missing"));
}
