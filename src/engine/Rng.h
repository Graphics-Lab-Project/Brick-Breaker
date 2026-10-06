#pragma once
// OWNER: Phase 0. FROZEN. Deterministic RNG (xorshift32). Never use std::rand / time seeds in logic.
#include <cstdint>
#include <functional>

namespace BB {

// Returns an int in [0, n). Tests inject scripted versions of this.
using RandFn = std::function<int(int)>;

class Rng {
public:
    explicit Rng(std::uint32_t seed = 0x9E3779B9u) : m_state(seed ? seed : 1u) {}
    std::uint32_t next()
    {
        std::uint32_t x = m_state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        m_state = x;
        return x;
    }
    int nextInt(int n) { return n <= 0 ? 0 : int(next() % std::uint32_t(n)); }
    RandFn fn() { return [this](int n) { return nextInt(n); }; }

private:
    std::uint32_t m_state;
};

} // namespace BB
