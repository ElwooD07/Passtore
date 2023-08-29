#include "pch.h"
#include "Cryptor.h"

using namespace passtore;

namespace
{
    uint64_t PowerOf2FastModulus(uint64_t target, uint64_t powerOf2)
    {
        int mask = powerOf2 - 1;
        return target & mask;
    }
}

passtore::Cryptor::Cryptor()
{ }

passtore::Cryptor::~Cryptor()
{
    // Avoid sudden keys leak to another process
    memset(m_keyAndIv.data(), 0, m_keyAndIv.size());
}

Cryptor::Cryptor(const Data& keyAndIv)
    : m_keyAndIv(keyAndIv)
{
    CheckKeys();
}

passtore::Cryptor::Cryptor(Data&& keyAndIv)
    : m_keyAndIv(std::move(keyAndIv))
{
    CheckKeys();
}

void Cryptor::SetKeyAndIv(const Data& keyAndIv)
{
    CheckKeys(keyAndIv);
    m_keyAndIv = keyAndIv;
}

void passtore::Cryptor::SetKeyAndIv(Data&& keyAndIv)
{
    CheckKeys(keyAndIv);
    m_keyAndIv = std::move(keyAndIv);
}

void Cryptor::Encrypt(const std::string_view& in, Data& out)
{
    CheckKeys();
    Encrypt(m_keyAndIv, in, out);
}

void Cryptor::Encrypt(const QString& in, Data& out)
{
    CheckKeys();
    Encrypt(m_keyAndIv, in.toUtf8().constData(), out);
}

void Cryptor::Encrypt(const Data& in, Data& out)
{
    CheckKeys();
    Encrypt(m_keyAndIv, in, out);
}

QString Cryptor::DecryptAsQString(const Data& data)
{
    CheckKeys();
    return QString::fromUtf8(Decrypt(m_keyAndIv, data).constData());
}

std::string Cryptor::DecryptAsStdString(const Data& data)
{
    CheckKeys();
    return Decrypt(m_keyAndIv, data).constData();
}

void Cryptor::Encrypt(const Data& keyAndIv, const std::string_view& in, Data& out)
{
    assert(AES_KEYLEN * 2 == keyAndIv.size());

    AES_ctx ctx { };
    auto keyData = reinterpret_cast<const uint8_t*>(keyAndIv.data());
    AES_init_ctx_iv(&ctx, keyData, keyData + AES_KEYLEN);

    out.resize(in.size() + 1);
    memcpy(&out[0], in.data(), in.size());
    out[out.size() - 1] = '\0';
    auto blockModulus = PowerOf2FastModulus(in.size() + 1, AES_BLOCKLEN);
    if (blockModulus != 0)
    {
        auto paddingSize = AES_BLOCKLEN - blockModulus;
        out.resize(in.size() + 1 + paddingSize);
    }
    AES_CBC_encrypt_buffer(&ctx, out.data(), static_cast<uint32_t>(out.size()));
}

void Cryptor::Encrypt(const Data& keyAndIv, const Data& in, Data& out)
{
    assert(AES_KEYLEN * 2 == keyAndIv.size());

    AES_ctx ctx { };
    auto keyData = reinterpret_cast<const uint8_t*>(keyAndIv.data());
    AES_init_ctx_iv(&ctx, keyData, keyData + AES_KEYLEN);

    out = in;
    auto blockModulus = PowerOf2FastModulus(in.size(), AES_BLOCKLEN);
    if (blockModulus != 0)
    {
        auto paddingSize = AES_BLOCKLEN - blockModulus;
        out.resize(in.size() + paddingSize);
    }
    AES_CBC_encrypt_buffer(&ctx, out.data(), static_cast<uint32_t>(out.size()));
}

QByteArray Cryptor::Decrypt(const Data& keyAndIv, const Data& data)
{
    assert(AES_KEYLEN * 2 == keyAndIv.size());

    AES_ctx ctx { };
    auto keyData = reinterpret_cast<const uint8_t*>(keyAndIv.data());
    AES_init_ctx_iv(&ctx, keyData, keyData + AES_KEYLEN);

    QByteArray result(reinterpret_cast<const char*>(data.data()), data.size());
    auto blockModulus = PowerOf2FastModulus(data.size(), AES_BLOCKLEN);
    if (blockModulus != 0)
    {
        auto paddingSize = AES_BLOCKLEN - blockModulus;
        result.resize(data.size() + paddingSize);
    }
    AES_CBC_decrypt_buffer(&ctx, reinterpret_cast<uint8_t*>(result.data()), result.size());
    return result;
}

const passtore::Data& passtore::Cryptor::GetKeyAndIv() const
{
    return m_keyAndIv;
}

void passtore::Cryptor::GenerateRandomKeyAndIv(Data& data)
{
    GenerateRandomSequence(AES_KEYLEN * 2, data);
}

void passtore::Cryptor::CheckKeys(const Data& keyAndIv)
{
    if (keyAndIv.size() != AES_KEYLEN * 2)
    {
        throw std::runtime_error(QObject::tr("Cryptor is not initialized with proper AES keys").toStdString());
    }
}

void passtore::Cryptor::CheckKeys() const
{
    CheckKeys(m_keyAndIv);
}

void passtore::GenerateRandomSequence(size_t sequenceLen, Data& data)
{
    data.resize(sequenceLen);
    std::random_device rnd;
    for (auto i = 0; i < sequenceLen--; ++i)
    {
        data[i] = static_cast<uint8_t>(PowerOf2FastModulus(rnd(), 256));
    }
}
