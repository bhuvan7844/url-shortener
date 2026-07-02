#include "PostgresStorage.h"
#include <iostream>

PostgresStorage::PostgresStorage(const std::string& connString)
    : connStr(connString) {}

bool PostgresStorage::save(const URLEntry& entry) {
    try {
        pqxx::connection c(connStr);
        pqxx::work txn(c);
        txn.exec(
            "INSERT INTO urls (short_code, long_url) VALUES (" +
            txn.quote(entry.shortCode) + ", " +
            txn.quote(entry.longURL) + ")"
        );
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "save() error: " << e.what() << "\n";
        return false;
    }
}

std::optional<URLEntry> PostgresStorage::get(const std::string& code) {
    try {
        pqxx::connection c(connStr);
        pqxx::work txn(c);
        auto result = txn.exec(
            "SELECT short_code, long_url, clicks FROM urls WHERE short_code = " +
            txn.quote(code)
        );
        if (result.empty()) return std::nullopt;

        auto row = result[0];
        URLEntry entry;
        entry.shortCode = row["short_code"].as<std::string>();
        entry.longURL   = row["long_url"].as<std::string>();
        entry.clicks    = row["clicks"].as<int>();
        return entry;
    } catch (const std::exception& e) {
        std::cerr << "get() error: " << e.what() << "\n";
        return std::nullopt;
    }
}

bool PostgresStorage::incrementClicks(const std::string& code) {
    try {
        pqxx::connection c(connStr);
        pqxx::work txn(c);
        auto result = txn.exec(
            "UPDATE urls SET clicks = clicks + 1 WHERE short_code = " +
            txn.quote(code)
        );
        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "incrementClicks() error: " << e.what() << "\n";
        return false;
    }
}

int64_t PostgresStorage::getMaxId() {
    try {
        pqxx::connection c(connStr);
        pqxx::work txn(c);
        auto result = txn.exec("SELECT COALESCE(MAX(id), 0) FROM urls");
        return result[0][0].as<int64_t>();
    } catch (const std::exception& e) {
        std::cerr << "getMaxId() error: " << e.what() << "\n";
        return 0;
    }
}
