#pragma once

namespace passtore
{
    using Data = std::vector<uint8_t>;

    namespace utils
    {
        void Sha256Calculate(const void* data, size_t size, Data& hash);
        Data Sha256Calculate(const void* data, size_t size);
    }
}
