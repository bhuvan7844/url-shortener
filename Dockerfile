# ── Stage 1: Build ──────────────────────────────────────────────────────────
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    libpqxx-dev \
    libpq-dev \
    curl \
    unzip \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY include/ include/
COPY src/     src/
COPY tests/   tests/
COPY CMakeLists.txt .

RUN mkdir build && cd build && \
    cmake .. -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_FLAGS="-O2" && \
    ninja url_shortener

# ── Stage 2: Runtime ─────────────────────────────────────────────────────────
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    libpqxx-6.4 \
    libpq5 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/url_shortener .
COPY public/ public/

EXPOSE 8080

CMD ["./url_shortener"]
