#ifndef BOMBOWE_ROBOTY_RANDOM_H
#define BOMBOWE_ROBOTY_RANDOM_H

#include <random>
#include <chrono>
#include <optional>

class Random {
private:
    uint32_t seed;
    std::minstd_rand random_gen;
public:
    explicit Random(std::optional<uint32_t> oseed)
        : seed(oseed.value_or(std::chrono::system_clock::now().time_since_epoch().count())),
          random_gen(seed) {}
    uint32_t operator()() {
        return static_cast<uint32_t>(random_gen());
    }
};

#endif //BOMBOWE_ROBOTY_RANDOM_H
