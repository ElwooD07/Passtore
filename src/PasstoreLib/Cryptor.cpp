#include "pch.h"
#include "Cryptor.h"
#include "aes.hpp"

passtore::Cryptor::Cryptor()
{ }

passtore::Cryptor::~Cryptor()
{
    // Avoid sudden keys leak to another process
    memset(m_keyAndIv.data(), 0, m_keyAndIv.size());
}

passtore::Cryptor::Cryptor(const QByteArray& keyAndIv)
{
    SetKeys(keyAndIv);
}

void passtore::Cryptor::SetKeys(const QByteArray& key, const QByteArray& iv)
{
    CheckData(key, iv);
    m_keyAndIv = key + iv;
}

void passtore::Cryptor::SetKeys(const QByteArray& keyAndIv)
{
    CheckData(keyAndIv);
    m_keyAndIv = keyAndIv;
}

void passtore::Cryptor::Encrypt(const std::string& in, Data& out)
{
    CheckData();
    Encrypt(m_keyAndIv, in, out);
}

void passtore::Cryptor::Encrypt(const QString& in, Data& out)
{
    CheckData();
    Encrypt(m_keyAndIv, in.toUtf8().constData(), out);
}

QByteArray passtore::Cryptor::Encrypt(const QString& text)
{
    CheckData();
    return Encrypt(text.toUtf8(), m_keyAndIv);
}

QByteArray passtore::Cryptor::Encrypt(const QByteArray& data)
{
    CheckData();
    return Encrypt(m_keyAndIv, data);
}

QByteArray passtore::Cryptor::Decrypt(const QByteArray& data)
{
    CheckData();
    return Decrypt(m_keyAndIv, data);
}

QString passtore::Cryptor::Decrypt(const Data& data)
{
    CheckData();
    return Decrypt(m_keyAndIv, data);
}

QString passtore::Cryptor::DecryptAsString(const QByteArray& data)
{
    CheckData();
    return QString::fromUtf8(Decrypt(m_keyAndIv, data));
}

QString passtore::Cryptor::DecryptAsString(const Data& data)
{
    CheckData();
    return QString::fromUtf8(Decrypt(m_keyAndIv, data));
}

std::string passtore::Cryptor::DecryptAsStdString(const Data& data)
{
    CheckData();
    auto out = Decrypt(m_keyAndIv, data);
    return out.toStdString();
}

void passtore::Cryptor::Encrypt(const QByteArray& keyAndIv, const std::string& in, Data& out)
{
    assert(AES_KEYLEN * 2 == keyAndIv.size());

    AES_ctx ctx { };
    auto keyData = reinterpret_cast<const uint8_t*>(keyAndIv.data());
    AES_init_ctx_iv(&ctx, keyData, keyData + AES_KEYLEN);

    out.resize(in.size() + (in.size() % AES_BLOCKLEN));
    AES_CBC_encrypt_buffer(&ctx, out.data(), static_cast<uint32_t>(out.size()));
}

QByteArray passtore::Cryptor::Encrypt(const QByteArray& keyAndIv, const QByteArray& data)
{
    assert(AES_KEYLEN * 2 == keyAndIv.size());

    AES_ctx ctx { };
    auto keyData = reinterpret_cast<const uint8_t*>(keyAndIv.data());
    AES_init_ctx_iv(&ctx, keyData, keyData + AES_KEYLEN);

    QByteArray result(data);
    result.resize(data.size() + (data.size() % AES_BLOCKLEN));
    AES_CBC_encrypt_buffer(&ctx, reinterpret_cast<uint8_t*>(result.data()), result.size());
    return result;
}

QByteArray passtore::Cryptor::Decrypt(const QByteArray& keyAndIv, const QByteArray& data)
{
    assert(AES_KEYLEN * 2 == keyAndIv.size());

    AES_ctx ctx { };
    auto keyData = reinterpret_cast<const uint8_t*>(keyAndIv.data());
    AES_init_ctx_iv(&ctx, keyData, keyData + AES_KEYLEN);

    QByteArray result(data);
    result.resize(data.size() + (data.size() % AES_BLOCKLEN));
    AES_CBC_decrypt_buffer(&ctx, reinterpret_cast<uint8_t*>(result.data()), result.size());
    return result;
}

QByteArray passtore::Cryptor::Decrypt(const QByteArray& keyAndIv, const Data& data)
{
    assert(AES_KEYLEN * 2 == keyAndIv.size());

    AES_ctx ctx { };
    auto keyData = reinterpret_cast<const uint8_t*>(keyAndIv.data());
    AES_init_ctx_iv(&ctx, keyData, keyData + AES_KEYLEN);

    QByteArray result(data.size(), 0);
    result.resize(data.size() + (data.size() % AES_BLOCKLEN));
    AES_CBC_decrypt_buffer(&ctx, reinterpret_cast<uint8_t*>(result.data()), result.size());
    return result;
}

QByteArrayView passtore::Cryptor::Key() const
{
    CheckData();
    return QByteArrayView(m_keyAndIv.data(), AES_KEYLEN);
}

QByteArrayView passtore::Cryptor::Iv() const
{
    CheckData();
    return QByteArrayView(m_keyAndIv.data() + AES_KEYLEN);
}

const QByteArray& passtore::Cryptor::KeyAndIv() const
{
    return m_keyAndIv;
}

QByteArray passtore::Cryptor::GenerateRandomKeyAndIv()
{
    return GenerateRandomSequence(AES_KEYLEN * 2);
}

void passtore::Cryptor::CheckData(const QByteArray& key, const QByteArray& iv)
{
    if (key.size() != AES_KEYLEN || iv.size() != AES_KEYLEN)
    {
        throw std::runtime_error(QObject::tr("Cryptor is not initialized with proper AES keys").toStdString());
    }
}

void passtore::Cryptor::CheckData(const QByteArray& keyAndIv)
{
    if (keyAndIv.size() != AES_KEYLEN * 2)
    {
        throw std::runtime_error(QObject::tr("Cryptor is not initialized with proper AES keys").toStdString());
    }
}

void passtore::Cryptor::CheckData() const
{
    CheckData(m_keyAndIv);
}

QByteArray passtore::GenerateRandomSequence(size_t sequenceLen)
{
    QByteArray result(sequenceLen, 0);
    srand(QDateTime::currentDateTimeUtc().toSecsSinceEpoch());
    for (auto i = 0; i < sequenceLen--; ++i)
    {
        result[i] = static_cast<char>(rand() % 255);
    }
    return result;
}
