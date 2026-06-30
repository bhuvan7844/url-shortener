#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00  // Windows 10+
#endif
#include "httplib.h"
#include "Encoder.h"
#include "PostgresStorage.h"
#include "TokenBucketLimiter.h"
#include <iostream>
#include <atomic>
#include <sstream>

std::string extractUrlField(const std::string& body) {
    auto pos = body.find("\"url\"");
    if (pos == std::string::npos) return "";
    pos = body.find(':', pos);
    auto firstQuote = body.find('"', pos + 1);
    auto secondQuote = body.find('"', firstQuote + 1);
    if (firstQuote == std::string::npos || secondQuote == std::string::npos) return "";
    return body.substr(firstQuote + 1, secondQuote - firstQuote - 1);
}

#include <cstdlib>

int main() {
    Encoder encoder;

    const char* dbConnEnv = std::getenv("DB_CONN");
    std::string connStr = dbConnEnv
        ? std::string(dbConnEnv)
        : "dbname=urlshortener user=postgres password=postgres123 host=localhost";

    PostgresStorage storage(connStr);

    // seed nextId from the current max in DB so we never collide on restart
    std::atomic<int64_t> nextId{storage.getMaxId() + 1};
    TokenBucketLimiter limiter(5, 0.5); // 5 burst, refills 0.5 tokens/sec

    httplib::Server svr;

    // serve static files — use PUBLIC_DIR env var or default to ./public
    const char* publicDir = std::getenv("PUBLIC_DIR");
    std::string publicPath = publicDir ? std::string(publicDir) : "./public";
    svr.set_mount_point("/", publicPath);

    svr.Post("/shorten", [&](const httplib::Request& req, httplib::Response& res) {
        std::string clientIp = req.remote_addr;
        if (!limiter.allow(clientIp)) {
            res.status = 429;
            res.set_content(R"({"error": "rate limit exceeded, slow down"})", "application/json");
            return;
        }

        std::string longUrl = extractUrlField(req.body);

        if (longUrl.empty() || longUrl.find("http") != 0) {
            res.status = 400;
            res.set_content(R"({"error": "invalid or missing url"})", "application/json");
            return;
        }

        int64_t id = nextId++;
        std::string code = encoder.encode(id);

        URLEntry entry{code, longUrl, 0, std::chrono::system_clock::now()};
        bool ok = storage.save(entry);

        if (!ok) {
            res.status = 500;
            res.set_content(R"({"error": "failed to save"})", "application/json");
            return;
        }

        std::ostringstream json;
        const char* baseUrlEnv = std::getenv("BASE_URL");
        std::string baseUrl = baseUrlEnv ? std::string(baseUrlEnv) : "http://localhost:8080";
        json << "{\"short_code\": \"" << code << "\", "
             << "\"short_url\": \"" << baseUrl << "/" << code << "\"}";

        res.set_content(json.str(), "application/json");
    });

    // redirect route — must come after static mount so /index.html still works
    svr.Get(R"(/([\w]+))", [&](const httplib::Request& req, httplib::Response& res) {
        std::string code = req.matches[1];
        auto entry = storage.get(code);

        if (!entry) {
            res.status = 404;
            res.set_content(R"({"error": "short code not found"})", "application/json");
            return;
        }

        storage.incrementClicks(code);
        res.set_redirect(entry->longURL);
    });

    std::cout << "Server running on http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);

    return 0;
}
