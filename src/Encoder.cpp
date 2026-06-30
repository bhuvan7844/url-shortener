// Encoder.cpp
#include "Encoder.h"
#include <algorithm>
#include <stdexcept>

const std::string Encoder::ALPHABET =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

std::string Encoder::encode(int64_t id) {
    if (id == 0) return "0";
    std::string result;
    while (id > 0) {
        result += ALPHABET[id % 62];
        id /= 62;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

int64_t Encoder::decode(const std::string& code) {
    int64_t id = 0;
    for (char c : code) {
        size_t pos = ALPHABET.find(c);
        if (pos == std::string::npos) throw std::invalid_argument("Invalid character in code");
        id = id * 62 + pos;
    }
    return id;
}