#ifndef BOMBOWE_ROBOTY_RANDOM_H
#define BOMBOWE_ROBOTY_RANDOM_H

#include <random>
#include <chrono>
#include <optional>

class Random {
private:
    uint32_t seed;
#ifdef USESTD_RAND
    std::minstd_rand random_gen;
#else
    uint64_t result;
#endif
public:
    explicit Random(std::optional<uint32_t> oseed)
        : seed(oseed.value_or(std::chrono::system_clock::now().time_since_epoch().count())),
#ifdef USESTD_RAND
          random_gen(seed)
#else
          result((seed * 48271) % 2147483647)
#endif
          {}
    uint32_t operator()() {
        result = (result * 48271) % 2147483647;
#ifdef USESTD_RAND
        return static_cast<uint32_t>(random_gen());
#else
        return static_cast<uint32_t>(result);
#endif
    }
};

#endif //BOMBOWE_ROBOTY_RANDOM_H
