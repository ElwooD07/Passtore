#pragma once
#include <string>
#include <vector>
#include <QString>
#include "Utils/DataUtils.h"

// TODO: TEST IT!

namespace passtore
{
    class Cryptor
    {
    public:
        Cryptor();
        ~Cryptor();
        explicit Cryptor(const Data& keyAndIv);
        explicit Cryptor(Data&& keyAndIv);

        void SetKeyAndIv(const Data& keyAndIv);
        void SetKeyAndIv(Data&& keyAndIv);

        // Methods are virtual for decorators
        virtual void Encrypt(const std::string_view& in, Data& out);
        virtual void Encrypt(const QString& in, Data& out);
        virtual void Encrypt(const Data& in, Data& out);

        virtual QString DecryptAsQString(const Data& data);
        virtual std::string DecryptAsStdString(const Data& data);

        static void Encrypt(const Data& keyAndIv, const std::string_view& in, Data& out);
        static void Encrypt(const Data& keyAndIv, const Data& in, Data& out);
        static QByteArray Decrypt(const Data& keyAndIv, const Data& data);

        const Data& GetKeyAndIv() const;

        static void GenerateRandomKeyAndIv(Data& data);

    private:
        static void CheckKeys(const Data& keyAndIv);
        void CheckKeys() const;

    private:
        Data m_keyAndIv;
    };

    void GenerateRandomSequence(size_t sequenceLen, Data& data);
    inline uint64_t Rotate64(uint64_t value, int steps)
    {
      const unsigned int mask = (CHAR_BIT*sizeof(value) - 1);
      steps &= mask;
      return (value>>steps) | (value<<( (-steps)&mask ));
    }
}
