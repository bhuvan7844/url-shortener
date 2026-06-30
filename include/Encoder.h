// Encoder.h
#pragma once
#include <string>
#include <cstdint>

class Encoder {
public:
    std::string encode(int64_t id);
    int64_t decode(const std::string& code);

private:
    static const std::string ALPHABET;
};