#pragma once
#include <cstdint>
#include <span>
#include <vector>

namespace passtore
{
    using Data = std::vector<uint8_t>;
    using Secret = std::span<const uint8_t>;

    namespace utils
    {
        void Sha256Calculate(const void* data, size_t size, Data& hash);
        Data Sha256Calculate(const void* data, size_t size);
    }
}
