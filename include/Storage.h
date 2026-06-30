#pragma once
#include "URLEntry.h"
#include <optional>
#include <string>

class Storage {
public:
    virtual ~Storage() = default;
    virtual bool save(const URLEntry& entry) = 0;
    virtual std::optional<URLEntry> get(const std::string& code) = 0;
    virtual bool incrementClicks(const std::string& code) = 0;
};