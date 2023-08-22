#include "pch.h"
#include "CellsCryptor.h"

void passtore::CellsCryptor::SetKeysAndSeed(const QByteArray& keyAndIv, uint64_t seed)
{
    m_cryptor.SetKeys(keyAndIv);
    m_seed = seed;
}

QByteArray passtore::CellsCryptor::Encrypt(const QString& text, int row, int column)
{
    const auto seed = Rotate64(m_seed + row, column);
    const auto seedData = QByteArrayView(reinterpret_cast<const char*>(&seed), sizeof(seed));
    QByteArray passwordHash = QCryptographicHash::hash(seedData, QCryptographicHash::Sha256);

    // TODO
    return {};
}

QString passtore::CellsCryptor::Decrypt(const QByteArray& data, int row, int column)
{
    // TODO
    return {};
}
