// InMemoryStorage.cpp
#include "InMemoryStorage.h"

bool InMemoryStorage::save(const URLEntry& entry) {
    std::lock_guard<std::mutex> lock(mtx);
    data[entry.shortCode] = entry;
    return true;
}

std::optional<URLEntry> InMemoryStorage::get(const std::string& code) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = data.find(code);
    if (it == data.end()) return std::nullopt;
    return it->second;
}

bool InMemoryStorage::incrementClicks(const std::string& code) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = data.find(code);
    if (it == data.end()) return false;
    it->second.clicks++;
    return true;
}