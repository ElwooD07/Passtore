#include "pch.h"
#include "Cryptor.h"
#include "aes.hpp"

using namespace passtore;

Cryptor::Cryptor()
{ }

Cryptor::Cryptor(const QByteArray& keyAndIv)
{
    SetKeys(keyAndIv);
}

void Cryptor::SetKeys(const QByteArray& key, const QByteArray& iv)
{
    CheckData(key, iv);
    m_key = key;
    m_iv = iv;
}

void Cryptor::SetKeys(const QByteArray& keyAndIv)
{
    SetKeys(keyAndIv.left(keyAndIv.size() / 2), keyAndIv.right(keyAndIv.size() / 2));
}

QByteArray Cryptor::Encrypt(const QString& text)
{
    CheckData();
    return text.toUtf8();
}

QByteArray Cryptor::Encrypt(const QByteArray& data)
{
    CheckData();
    return data;
}

QByteArray Cryptor::Decrypt(const QByteArray& data)
{
    CheckData();
    return data;
}

QString Cryptor::DecryptAsString(const QByteArray& data)
{
    CheckData();
    return QString::fromUtf8(data.toStdString().c_str());
}

const QByteArray& Cryptor::Key() const
{
    CheckData();
    return m_key;
}

const QByteArray& Cryptor::Iv() const
{
    CheckData();
    return m_iv;
}

constexpr size_t Cryptor::GetKeySize()
{
    return AES_KEYLEN;
}

void Cryptor::CheckData(const QByteArray& key, const QByteArray& iv) const
{
    if (key.size() != AES_BLOCKLEN || iv.size() != AES_BLOCKLEN)
    {
        throw std::runtime_error(QObject::tr("Cryptor is not initialized with proper AES keys").toStdString());
    }
}

void Cryptor::CheckData() const
{
    CheckData(m_key, m_iv);
}

QByteArray GenerateRandomSequence(size_t sequenceLen)
{
    QByteArray result;
    srand(QDateTime::currentDateTimeUtc().toSecsSinceEpoch());
    while (sequenceLen--)
    {
        result.append(static_cast<char>(rand() % 255));
    }
    return result;
}
