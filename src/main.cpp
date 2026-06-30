#include "Encoder.h"
#include "InMemoryStorage.h"
#include <iostream>

int main() {
    Encoder encoder;
    InMemoryStorage storage;

    int64_t nextId = 1;
    std::string code = encoder.encode(nextId);
    std::cout << "Encoded ID " << nextId << " -> " << code << "\n";

    URLEntry entry{code, "https://example.com", 0, std::chrono::system_clock::now()};
    storage.save(entry);

    auto result = storage.get(code);
    if (result) {
        std::cout << "Retrieved: " << result->longURL << "\n";
    }

    int64_t decoded = encoder.decode(code);
    std::cout << "Decoded back to ID: " << decoded << "\n";

    return 0;
}