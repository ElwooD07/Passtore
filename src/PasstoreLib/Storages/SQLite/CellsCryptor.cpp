#include "pch.h"
#include "CellsCryptor.h"
#include "Utils/sha256.h"

using namespace passtore;

void sqlite::CellsCryptor::SetKeysAndSeed(const Data& keyAndIv, uint64_t seed)
{
    m_keyAndIv = keyAndIv;
    m_seed = seed;
}

/*void sqlite::CellsCryptor::Encrypt(const QString& text, int row, int column, Data& data)
{
    Data keys;
    GetKeysForCell(row, column, keys);
    Cryptor cryptor(std::move(keys));

    cryptor.Encrypt(text, data);
}

QString sqlite::CellsCryptor::Decrypt(const QByteArray& data, int row, int column)
{
    Data keys;
    GetKeysForCell(row, column, keys);
    Cryptor cryptor(std::move(keys));

    return cryptor.DecryptAsString(data);
}*/

void sqlite::CellsCryptor::GetKeysForCell(int row, int column, Data& keys)
{
    const auto seed = Rotate64(m_seed + row, column);
    Data seedHash(SHA256_HASH_SIZE, 0);
    Sha256Calculate(&seed, sizeof(seed), reinterpret_cast<SHA256_HASH*>(&seedHash[0]));

    MixKeys(m_keyAndIv, seedHash);
}

void sqlite::CellsCryptor::MixKeys(const Data& spice, Data& dish)
{
    using portion = uint64_t;
    assert(dish.size() == spice.size());
    assert(dish.size() % sizeof(portion) == 0);

    for (auto i = 0; i < dish.size() / sizeof(portion); ++i)
    {
        auto target = reinterpret_cast<portion*>(dish.data()[i*sizeof(portion)]);
        *target += *reinterpret_cast<portion*>(spice.data()[i*sizeof(portion)]);
    }
}
