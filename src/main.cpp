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

int main() {
    Encoder encoder;
    PostgresStorage storage("dbname=urlshortener user=postgres password=postgres123 host=localhost");

    // seed nextId from the current max in DB so we never collide on restart
    std::atomic<int64_t> nextId{storage.getMaxId() + 1};
    TokenBucketLimiter limiter(5, 0.5); // 5 burst, refills 0.5 tokens/sec

    httplib::Server svr;

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
        json << "{\"short_code\": \"" << code << "\", "
             << "\"short_url\": \"http://localhost:8080/" << code << "\"}";

        res.set_content(json.str(), "application/json");
    });

    svr.Get(R"(/(\w+))", [&](const httplib::Request& req, httplib::Response& res) {
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
