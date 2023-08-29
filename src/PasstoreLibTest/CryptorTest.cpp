#include "pch.h"
#include "Security/Cryptor.h"

using namespace passtore;

Cryptor CreateCryptor()
{
    Data keys;
    Cryptor::GenerateRandomKeyAndIv(keys);
    return Cryptor(std::move(keys));
}

TEST(CryptorTest, EncryptDecryptToStdStringTest)
{
    auto cryptor = CreateCryptor();
    std::string phrase("TestString");

    Data encrypted;
    EXPECT_NO_THROW(cryptor.Encrypt(phrase, encrypted));
    ASSERT_FALSE(encrypted.empty());
    ASSERT_NE(0, memcmp(phrase.data(), encrypted.data(), std::min(phrase.size(), encrypted.size())));

    auto decrypted = cryptor.DecryptAsStdString(encrypted);
    ASSERT_EQ(phrase.size(), decrypted.size());
    EXPECT_STREQ(phrase.c_str(), decrypted.c_str());
}

TEST(CryptorTest, EncryptDecryptToQStringTest)
{
    auto cryptor = CreateCryptor();
    std::string phrase("TestString");

    Data encrypted;
    EXPECT_NO_THROW(cryptor.Encrypt(phrase, encrypted));
    ASSERT_FALSE(encrypted.empty());
    ASSERT_NE(0, memcmp(phrase.data(), encrypted.data(), std::min(phrase.size(), encrypted.size())));

    auto decrypted = cryptor.DecryptAsQString(encrypted);
    ASSERT_EQ(phrase.size(), decrypted.size());
    EXPECT_STREQ(phrase.c_str(), decrypted.toUtf8().constData());
}
