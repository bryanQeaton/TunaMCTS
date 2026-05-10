#ifndef TUNAMCTS_TT_H
#define TUNAMCTS_TT_H

#define PREFETCH(addr) __builtin_prefetch(addr, 0, 3)
#include <cstdint>
#include <cstring>


// Each entry is forced to 64 bytes (cache line size)
struct alignas(64) Entry {
    uint64_t hash=0;
    float value=0.f;
    uint64_t visits=0;
    bool operator==(const Entry& other) const = default;
};
static_assert(sizeof(Entry) == 64, "Entry size must be 64 bytes");
constexpr size_t TOTAL_ENTRIES = (HASH_SIZE * 1024ULL * 1024ULL) / sizeof(Entry);
constexpr size_t BUCKET_COUNT = 1ULL << (64 - __builtin_clzll((TOTAL_ENTRIES / BUCKET_SIZE) - 1));
static_assert((BUCKET_COUNT & (BUCKET_COUNT - 1)) == 0, "Bucket count must be power of two");
constexpr uint64_t MASK = BUCKET_COUNT - 1;
struct TT {
    Entry table[BUCKET_COUNT][BUCKET_SIZE]{};
    uint8_t replace_idx[BUCKET_COUNT]{};
    const Entry* get_bucket(uint64_t hash) const {
        uint64_t idx = hash & MASK;
        return table[idx];
    }
    void clear() {
        for (auto& bucket : table)
            for (auto& entry : bucket)
                entry = Entry();
        std::memset(replace_idx, 0, sizeof(replace_idx));
    }
    Entry& operator[](uint64_t hash) {
        uint64_t idx = hash & MASK;
        Entry* bucket = table[idx];
        for (int i = 0; i < BUCKET_SIZE; ++i) {
            if (bucket[i].hash == hash)
                return bucket[i];
        }
        int replace = replace_idx[idx];
        replace_idx[idx] = (replace_idx[idx] + 1) % BUCKET_SIZE;
        return bucket[replace];
    }
};
inline auto tt = TT();



#endif //TUNAMCTS_TT_H
