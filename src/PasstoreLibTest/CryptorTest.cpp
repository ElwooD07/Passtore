#include "pch.h"
#include "Security/Cryptor.h"
#include "Security/SensitiveData.h"

using namespace passtore;

Cryptor CreateCryptor()
{
    Data keys;
    Cryptor::GenerateRandomKeyAndIv(keys);
    return Cryptor(Secret(keys.data(), keys.size()));
}

TEST(CryptorTest, EncryptDecryptToSensitiveDataTest)
{
    auto cryptor = CreateCryptor();
    const std::string phrase("TestString");
    const auto phraseBytes = Secret(reinterpret_cast<const uint8_t*>(phrase.data()), phrase.size());

    Data encrypted;
    EXPECT_NO_THROW(cryptor.Encrypt(phraseBytes, encrypted));
    ASSERT_FALSE(encrypted.empty());
    ASSERT_NE(0, memcmp(phrase.data(), encrypted.data(), std::min(phrase.size(), encrypted.size())));

    SensitiveData decrypted;
    EXPECT_NO_THROW(cryptor.Decrypt(encrypted, decrypted));
    auto decryptedView = decrypted.View();
    size_t decryptedLen = 0;
    while (decryptedLen < decryptedView.size() && decryptedView[decryptedLen] != '\0')
    {
        ++decryptedLen;
    }

    ASSERT_EQ(phrase.size(), decryptedLen);
    EXPECT_EQ(0, memcmp(phrase.data(), decryptedView.data(), phrase.size()));
}
