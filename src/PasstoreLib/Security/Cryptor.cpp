#include "pch.h"
#include "Cryptor.h"
#include "Security/SecureMemory.h"
#include "aes.hpp"

using namespace passtore;

namespace
{
    uint64_t PowerOf2FastModulus(uint64_t target, uint64_t powerOf2)
    {
        int mask = powerOf2 - 1;
        return target & mask;
    }
}

void Cryptor::Encrypt(Secret keyAndIv, Secret in, Data& out)
{
    CheckKeys(keyAndIv);

    AES_ctx ctx { };
    auto keyData = keyAndIv.data();
    AES_init_ctx_iv(&ctx, keyData, keyData + AES_KEYLEN);

    out.assign(in.begin(), in.end());
    out.push_back('\0');
    auto blockModulus = PowerOf2FastModulus(out.size(), AES_BLOCKLEN);
    if (blockModulus != 0)
    {
        auto paddingSize = AES_BLOCKLEN - blockModulus;
        out.resize(out.size() + paddingSize);
    }
    AES_CBC_encrypt_buffer(&ctx, out.data(), static_cast<uint32_t>(out.size()));
}

void Cryptor::Decrypt(Secret keyAndIv, const Data& data, SensitiveData& out)
{
    CheckKeys(keyAndIv);

    AES_ctx ctx { };
    auto keyData = keyAndIv.data();
    AES_init_ctx_iv(&ctx, keyData, keyData + AES_KEYLEN);

    Data result(data.begin(), data.end());
    auto blockModulus = PowerOf2FastModulus(data.size(), AES_BLOCKLEN);
    if (blockModulus != 0)
    {
        auto paddingSize = AES_BLOCKLEN - blockModulus;
        result.resize(data.size() + paddingSize);
    }
    AES_CBC_decrypt_buffer(&ctx, result.data(), static_cast<uint32_t>(result.size()));
    out.Assign(Secret(result.data(), result.size()));
    SecureWipe(result.data(), result.size());
}

void passtore::Cryptor::GenerateRandomKeyAndIv(Data& data)
{
    GenerateRandomSequence(AES_KEYLEN * 2, data);
}

void passtore::Cryptor::CheckKeys(Secret keyAndIv)
{
    if (keyAndIv.size() != AES_KEYLEN * 2)
    {
        throw std::runtime_error("Cryptor is not initialized with proper AES keys");
    }
}

void passtore::GenerateRandomSequence(size_t sequenceLen, Data& data)
{
    data.resize(sequenceLen);
    std::random_device rnd;
    for (size_t i = 0; i < sequenceLen; ++i)
    {
        data[i] = static_cast<uint8_t>(PowerOf2FastModulus(rnd(), 256));
    }
}
