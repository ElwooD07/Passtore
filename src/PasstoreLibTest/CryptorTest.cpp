#include "pch.h"
#include "Security/Cryptor.h"
#include "Security/SensitiveData.h"

using namespace passtore;

namespace
{
    SensitiveData CreateKey()
    {
        Data rawKey;
        Cryptor::GenerateRandomKeyAndIv(rawKey);
        SensitiveData key;
        key.Assign(Secret(rawKey.data(), rawKey.size()));
        SecureWipe(rawKey.data(), rawKey.size());
        return key;
    }
}

TEST(CryptorTest, EncryptDecryptToSensitiveDataTest)
{
    auto key = CreateKey();

    const std::string phrase("TestString");
    const auto phraseBytes = Secret(reinterpret_cast<const uint8_t*>(phrase.data()), phrase.size());

    Data encrypted;
    EXPECT_NO_THROW(Cryptor::Encrypt(key.View(), phraseBytes, encrypted));
    ASSERT_FALSE(encrypted.empty());
    const size_t compareLen = phrase.size() < encrypted.size() ? phrase.size() : encrypted.size();
    ASSERT_NE(0, memcmp(phrase.data(), encrypted.data(), compareLen));

    SensitiveData decrypted;
    EXPECT_NO_THROW(Cryptor::Decrypt(key.View(), encrypted, decrypted));
    auto decryptedView = decrypted.View();
    size_t decryptedLen = 0;
    while (decryptedLen < decryptedView.size() && decryptedView[decryptedLen] != '\0')
    {
        ++decryptedLen;
    }

    ASSERT_EQ(phrase.size(), decryptedLen);
    EXPECT_EQ(0, memcmp(phrase.data(), decryptedView.data(), phrase.size()));
}

TEST(CryptorTest, EncryptThrowsOnInvalidKeySize)
{
    Data invalidKey(5, 0x11);
    const std::string phrase("TestString");
    const auto phraseBytes = Secret(reinterpret_cast<const uint8_t*>(phrase.data()), phrase.size());
    Data encrypted;

    EXPECT_THROW(Cryptor::Encrypt(Secret(invalidKey.data(), invalidKey.size()), phraseBytes, encrypted), std::runtime_error);
}

TEST(CryptorTest, DecryptThrowsOnInvalidKeySize)
{
    Data invalidKey(5, 0x11);
    Data encrypted(16, 0x00);
    SensitiveData decrypted;

    EXPECT_THROW(Cryptor::Decrypt(Secret(invalidKey.data(), invalidKey.size()), encrypted, decrypted), std::runtime_error);
}

TEST(CryptorTest, EncryptDecryptEmptyString)
{
    auto key = CreateKey();
    const std::string phrase;
    const auto phraseBytes = Secret(reinterpret_cast<const uint8_t*>(phrase.data()), phrase.size());

    Data encrypted;
    EXPECT_NO_THROW(Cryptor::Encrypt(key.View(), phraseBytes, encrypted));
    ASSERT_FALSE(encrypted.empty());

    SensitiveData decrypted;
    EXPECT_NO_THROW(Cryptor::Decrypt(key.View(), encrypted, decrypted));
    auto decryptedView = decrypted.View();
    ASSERT_FALSE(decryptedView.empty());
    EXPECT_EQ('\0', decryptedView[0]);
}

TEST(CryptorTest, EncryptDecryptBinaryDataWithZeroBytes)
{
    auto key = CreateKey();
    Data binary { 0x00, 0x01, 0x02, 0x00, 0xFF, 0x10, 0x00, 0x20 };

    Data encrypted;
    EXPECT_NO_THROW(Cryptor::Encrypt(key.View(), Secret(binary.data(), binary.size()), encrypted));
    ASSERT_FALSE(encrypted.empty());

    SensitiveData decrypted;
    EXPECT_NO_THROW(Cryptor::Decrypt(key.View(), encrypted, decrypted));
    auto decryptedView = decrypted.View();

    ASSERT_GE(decryptedView.size(), binary.size());
    EXPECT_EQ(0, memcmp(binary.data(), decryptedView.data(), binary.size()));
}
