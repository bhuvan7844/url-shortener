CREATE TABLE IF NOT EXISTS urls (
    id         BIGSERIAL PRIMARY KEY,
    short_code VARCHAR(10) UNIQUE NOT NULL,
    long_url   TEXT NOT NULL,
    clicks     INT DEFAULT 0,
    created_at TIMESTAMP DEFAULT NOW()
);
