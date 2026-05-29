#pragma once
#include <cstdint>
#include <span>
#include <vector>
#include "Security/SecureMemory.h"

namespace passtore
{
    /* std::vector<uint8_t> that securely wipes its contents on destruction. */
    class Data : public std::vector<uint8_t>
    {
    public:
        using std::vector<uint8_t>::vector;
        ~Data() { SecureWipe(this->data(), this->size()); }
    };

    using Secret = std::span<const uint8_t>;

    namespace utils
    {
        void Sha256Calculate(const void* data, size_t size, Data& hash);
        Data Sha256Calculate(const void* data, size_t size);
    }
}
