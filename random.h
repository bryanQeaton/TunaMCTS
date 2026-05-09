#ifndef TUNAMCTS_RANDOM_H
#define TUNAMCTS_RANDOM_H
#include <cstdint>
#include <random>

static uint64_t sm_state=std::random_device{}();
inline uint64_t splitmix64() {
    uint64_t z = (sm_state += 0x9e3779b97f4a7c15ull);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
    return z ^ (z >> 31);
}
inline uint32_t fast_rand(uint32_t n){
    const auto x = static_cast<uint32_t>(splitmix64());
    return x % n;
}

#endif //TUNAMCTS_RANDOM_H
