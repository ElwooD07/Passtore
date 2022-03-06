#include "pch.h"
#include "Cryptor.h"
#include "aes.hpp"

Cryptor::Cryptor()
{ }

Cryptor::Cryptor(const QByteArray& keyAndIv)
{
    SetKeys(keyAndIv);
}

void Cryptor::SetKeys(const QByteArray& key, const QByteArray& iv)
{
    CheckKeys(key, iv);
    m_key = key;
    m_iv = iv;
}

void Cryptor::SetKeys(const QByteArray& keyAndIv)
{
    SetKeys(keyAndIv.left(keyAndIv.size() / 2), keyAndIv.right(keyAndIv.size() / 2));
}

QByteArray Cryptor::Encrypt(const QString& text)
{
    CheckKeys();
    return text.toUtf8();
}

QByteArray Cryptor::Encrypt(const QByteArray& data)
{
    CheckKeys();
    return data;
}

QByteArray Cryptor::Decrypt(const QByteArray& data)
{
    CheckKeys();
    return data;
}

QString Cryptor::DecryptAsString(const QByteArray& data)
{
    CheckKeys();
    return QString::fromUtf8(data.toStdString().c_str());
}

const QByteArray& Cryptor::Key() const
{
    CheckKeys();
    return m_key;
}

const QByteArray& Cryptor::Iv() const
{
    CheckKeys();
    return m_iv;
}

void Cryptor::CheckKeys(const QByteArray& key, const QByteArray& iv) const
{
    if (key.size() != AES_BLOCKLEN || iv.size() != AES_BLOCKLEN)
    {
        throw std::runtime_error(QObject::tr("Cryptor is not initialized with proper AES keys").toStdString());
    }
}

void Cryptor::CheckKeys() const
{
    CheckKeys(m_key, m_iv);
}
