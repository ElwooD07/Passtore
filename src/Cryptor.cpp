#include "pch.h"
#include "Cryptor.h"
#include "aes.hpp"

passtore::Cryptor::Cryptor()
{ }

passtore::Cryptor::Cryptor(const QByteArray& keyAndIv)
{
    SetKeys(keyAndIv);
}

void passtore::Cryptor::SetKeys(const QByteArray& key, const QByteArray& iv)
{
    CheckData(key, iv);
    m_key = key;
    m_iv = iv;
}

void passtore::Cryptor::SetKeys(const QByteArray& keyAndIv)
{
    SetKeys(keyAndIv.left(keyAndIv.size() / 2), keyAndIv.right(keyAndIv.size() / 2));
}

QByteArray passtore::Cryptor::Encrypt(const QString& text)
{
    CheckData();
    return text.toUtf8();
}

QByteArray passtore::Cryptor::Encrypt(const QByteArray& data)
{
    CheckData();
    return data;
}

QByteArray passtore::Cryptor::Decrypt(const QByteArray& data)
{
    CheckData();
    return data;
}

QString passtore::Cryptor::DecryptAsString(const QByteArray& data)
{
    CheckData();
    return QString::fromUtf8(data.toStdString().c_str());
}

const QByteArray& passtore::Cryptor::Key() const
{
    CheckData();
    return m_key;
}

const QByteArray& passtore::Cryptor::Iv() const
{
    CheckData();
    return m_iv;
}

size_t passtore::Cryptor::GetKeySize()
{
    return AES_KEYLEN;
}

void passtore::Cryptor::CheckData(const QByteArray& key, const QByteArray& iv) const
{
    if (key.size() != AES_BLOCKLEN || iv.size() != AES_BLOCKLEN)
    {
        throw std::runtime_error(QObject::tr("Cryptor is not initialized with proper AES keys").toStdString());
    }
}

void passtore::Cryptor::CheckData() const
{
    CheckData(m_key, m_iv);
}

QByteArray passtore::GenerateRandomSequence(size_t sequenceLen)
{
    QByteArray result;
    srand(QDateTime::currentDateTimeUtc().toSecsSinceEpoch());
    while (sequenceLen--)
    {
        result.append(static_cast<char>(rand() % 255));
    }
    return result;
}
