#include "fow/Shared/Rng.hpp"

namespace fow::Random {
    static std::mt19937 s_rng;

    void Seed(const uint64_t seed) {
        s_rng.seed(seed);
    }
    int32_t GetNextInt(const int32_t min, const int32_t max) {
        std::uniform_int_distribution dist(min, max);
        return dist(s_rng);
    }
    uint32_t GetNextUInt(const uint32_t min, const uint32_t max) {
        std::uniform_int_distribution dist(min, max);
        return dist(s_rng);
    }
    float GetNextFloat(const float min, const float max) {
        std::uniform_real_distribution dist(min, max);
        return dist(s_rng);
    }
    double GetNextDouble(const double min, const double max) {
        std::uniform_real_distribution dist(min, max);
        return dist(s_rng);
    }
    bool GetNextBool(const float weight) {
        std::bernoulli_distribution dist(weight);
        return dist(s_rng);
    }
}
