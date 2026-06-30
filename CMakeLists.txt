cmake_minimum_required(VERSION 3.16)
project(url_shortener)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(PostgreSQL REQUIRED)

# ---------- main executable ----------
add_executable(url_shortener
    src/main.cpp
    src/Encoder.cpp
    src/InMemoryStorage.cpp
    src/PostgresStorage.cpp
    src/TokenBucketLimiter.cpp
)

target_include_directories(url_shortener PRIVATE include)
target_link_libraries(url_shortener PRIVATE pqxx pq PostgreSQL::PostgreSQL Threads::Threads $<$<PLATFORM_ID:Windows>:ws2_32> $<$<PLATFORM_ID:Windows>:wsock32>)

# ---------- Google Test ----------
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(run_tests
    tests/test_encoder.cpp
    tests/test_storage.cpp
    tests/test_ratelimiter.cpp
    src/Encoder.cpp
    src/InMemoryStorage.cpp
    src/TokenBucketLimiter.cpp
)

target_include_directories(run_tests PRIVATE include)
target_link_libraries(run_tests PRIVATE GTest::gtest_main Threads::Threads)

include(GoogleTest)
gtest_discover_tests(run_tests)
