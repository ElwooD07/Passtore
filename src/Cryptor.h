#pragma once
#include <QByteArray>

namespace passtore
{
    class Cryptor
    {
    public:
        Cryptor();
        explicit Cryptor(const QByteArray& keyAndIv);

        void SetKeys(const QByteArray& key, const QByteArray& iv);
        void SetKeys(const QByteArray& keyAndIv);

        QByteArray Encrypt(const QString& text);
        QByteArray Encrypt(const QByteArray& data);
        QByteArray Decrypt(const QByteArray& data);
        QString DecryptAsString(const QByteArray& data);

        const QByteArray& Key() const;
        const QByteArray& Iv() const;

        static constexpr size_t GetKeySize();

    private:
        void CheckData(const QByteArray& key, const QByteArray& iv) const;
        void CheckData() const;

    private:
        QByteArray m_key;
        QByteArray m_iv;
    };

    QByteArray GenerateRandomSequence(size_t sequenceLen);
}
