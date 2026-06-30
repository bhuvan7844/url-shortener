# url-shortener

## Running Tests

```bash
cd build && cmake .. && ninja run_tests && ./run_tests
```

Covers: base62 encoder round-trip correctness (incl. collision testing up to 100k IDs), storage interface (save/get/increment), and token bucket rate limiter (burst limits, refill behavior, per-client isolation).