#pragma once
#include <vector>
#include <span>
#include "Utils/DataUtils.h"
#include "Security/SensitiveData.h"

// TODO: TEST IT!

namespace passtore
{
    // Uses AES 256 in CBC mode to encrypt/decrypt data and targeted on crypting strings,
    // so the end of decrypted message is determined by null symbol
    // It uses the pair of AES KEY and IV placed in the single container consecutively
    // You can use it as implementation or base class for your specific cryptor
    class Cryptor
    {
    public:
        Cryptor();
        ~Cryptor();
        explicit Cryptor(Secret keyAndIv);

        void SetKeyAndIv(Secret keyAndIv);

        // Methods are virtual for decorators
        virtual void Encrypt(Secret in, Data& out);
        virtual void Decrypt(const Data& data, SensitiveData& out);

        static void Encrypt(Secret keyAndIv, Secret in, Data& out);
        static void Decrypt(Secret keyAndIv, const Data& data, SensitiveData& out);

        Secret GetKeyAndIv() const;

        static void GenerateRandomKeyAndIv(Data& data);

    private:
        static void CheckKeys(Secret keyAndIv);
        void CheckKeys() const;

    private:
        SensitiveData m_keyAndIv;
    };

    void GenerateRandomSequence(size_t sequenceLen, Data& data);
    inline uint64_t Rotate64(uint64_t value, int steps)
    {
      const unsigned int mask = (CHAR_BIT*sizeof(value) - 1);
      steps &= mask;
      return (value>>steps) | (value<<( (-steps)&mask ));
    }
}
