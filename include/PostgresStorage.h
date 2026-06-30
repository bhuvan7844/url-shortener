#pragma once
#include "Storage.h"
#include <pqxx/pqxx>
#include <memory>

class PostgresStorage : public Storage {
public:
    explicit PostgresStorage(const std::string& connString);

    bool save(const URLEntry& entry) override;
    std::optional<URLEntry> get(const std::string& code) override;
    bool incrementClicks(const std::string& code) override;
    int64_t getMaxId();

private:
    std::unique_ptr<pqxx::connection> conn;
};
