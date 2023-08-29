#pragma once
#include "Security/Cryptor.h"

namespace passtore
{
    namespace sqlite
    {
        // Randomizes key and iv for each cell using seed
        class CellsCryptor
        {
        public:
            CellsCryptor();

            void SetKeysAndSeed(const Data& keyAndIv, uint64_t seed);

            //void Encrypt(const QString& text, int row, int column, Data& data);
            //QString Decrypt(const QByteArray& data, int row, int column);

        private:
            void GetKeysForCell(int row, int column, Data& keys);
            static void MixKeys(const Data& spice, Data& dish);

        private:
            Data m_keyAndIv;
            uint64_t m_seed = 0;
        };
    }
}

