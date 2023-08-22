#pragma once
#include "Cryptor.h"

namespace passtore
{
    // Randomizes key and iv for each cell using seed
    class CellsCryptor
    {
    public:
        CellsCryptor();

        void SetKeysAndSeed(const QByteArray& keyAndIv, uint64_t seed);

        QByteArray Encrypt(const QString& text, int row, int column);
        QString Decrypt(const QByteArray& data, int row, int column);

    private:
        Cryptor m_cryptor;
        uint64_t m_seed = 0;
    };
}

