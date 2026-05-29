#pragma once
#include <vector>
#include <span>
#include "Utils/DataUtils.h"
#include "Security/SensitiveData.h"

// TODO: TEST IT!

namespace passtore
{
    /* AES-256-CBC encrypt/decrypt helper; the null terminator marks the end of decrypted string data.
       Key and IV are stored consecutively in a single Secret span. */
    class Cryptor
    {
    public:
        static void Encrypt(Secret keyAndIv, Secret in, Data& out);
        static void Decrypt(Secret keyAndIv, const Data& data, SensitiveData& out);

        static void GenerateRandomKeyAndIv(Data& data);

    private:
        static void CheckKeys(Secret keyAndIv);
    };

    void GenerateRandomSequence(size_t sequenceLen, Data& data);
    inline uint64_t Rotate64(uint64_t value, int steps)
    {
      const unsigned int mask = (CHAR_BIT*sizeof(value) - 1);
      steps &= mask;
      return (value>>steps) | (value<<( (-steps)&mask ));
    }
}
