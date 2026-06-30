// InMemoryStorage.h
#pragma once
#include "Storage.h"
#include <unordered_map>
#include <mutex>

class InMemoryStorage : public Storage {
public:
    bool save(const URLEntry& entry) override;
    std::optional<URLEntry> get(const std::string& code) override;
    bool incrementClicks(const std::string& code) override;

private:
    std::unordered_map<std::string, URLEntry> data;
    std::mutex mtx;
};