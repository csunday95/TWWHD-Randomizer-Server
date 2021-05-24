
#pragma once

#include <cstdint>

namespace Utility
{

    uint64_t byteswap(const uint64_t& value);

    uint32_t byteswap(const uint32_t& value);

    uint16_t byteswap(const uint16_t& value);

    int64_t byteswap(const int64_t& value);

    int32_t byteswap(const int32_t& value);

    int16_t byteswap(const int16_t& value);

    float byteswap(const float& value);

    double byteswap(const double& value);

    template<typename T>
    void byteswap_inplace(T& value)
    {
        value = byteswap(value);
    }
}
