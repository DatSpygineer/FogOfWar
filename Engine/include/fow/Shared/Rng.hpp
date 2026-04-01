#ifndef FOW_RNG_HPP
#define FOW_RNG_HPP

#include <fow/Shared/Api.hpp>
#include <random>

namespace fow::Random {
    FOW_SHARED_API void Seed(uint64_t seed);
    FOW_SHARED_API int32_t GetNextInt(int32_t min, int32_t max);
    FOW_SHARED_API uint32_t GetNextUInt(uint32_t min, uint32_t max);
    FOW_SHARED_API float GetNextFloat(float min, float max);
    FOW_SHARED_API double GetNextDouble(double min, double max);
    FOW_SHARED_API bool GetNextBool(float weight = 0.5f);
}

#endif