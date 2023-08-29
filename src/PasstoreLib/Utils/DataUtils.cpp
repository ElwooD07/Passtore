#include "pch.h"
#include "Utils/DataUtils.h"
#include "Utils/sha256.h"

using namespace passtore;

void utils::Sha256Calculate(const void* data, size_t size, Data& hash)
{
    hash.resize(SHA256_HASH_SIZE);
    Sha256Calculate(data, size, reinterpret_cast<SHA256_HASH*>(&hash[0]));
}

Data utils::Sha256Calculate(const void* data, size_t size)
{
    Data hash;
    Sha256Calculate(data, size, hash);
    return hash;
}
