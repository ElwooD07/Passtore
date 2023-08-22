#pragma once
#include <QByteArray>

// TODO: move all security entities to the lib and test them
// TODO: TEST IT!

namespace passtore
{
    using Data = std::vector<uint8_t>;

    class Cryptor
    {
    public:
        Cryptor();
        ~Cryptor();
        explicit Cryptor(const QByteArray& keyAndIv);

        void SetKeys(const QByteArray& key, const QByteArray& iv);
        void SetKeys(const QByteArray& keyAndIv);

        // Methods are virtual for decorators
        virtual void Encrypt(const std::string& in, Data& out);
        virtual void Encrypt(const QString& in, Data& out);
        virtual QByteArray Encrypt(const QString& text);
        virtual QByteArray Encrypt(const QByteArray& data);
        virtual QByteArray Decrypt(const QByteArray& data);
        virtual QString Decrypt(const Data& data);
        virtual QString DecryptAsString(const QByteArray& data);
        virtual QString DecryptAsString(const Data& data);
        virtual std::string DecryptAsStdString(const Data& data);

        static void Encrypt(const QByteArray& keyAndIv, const std::string& in, Data& out);
        static QByteArray Encrypt(const QByteArray& keyAndIv, const QByteArray& data);
        static QByteArray Decrypt(const QByteArray& keyAndIv, const QByteArray& data);
        static QByteArray Decrypt(const QByteArray& keyAndIv, const Data& data);

        QByteArrayView Key() const;
        QByteArrayView Iv() const;
        const QByteArray& KeyAndIv() const;

        static QByteArray GenerateRandomKeyAndIv();

    private:
        static void CheckData(const QByteArray& key, const QByteArray& iv);
        static void CheckData(const QByteArray& keyAndIv);
        void CheckData() const;

    private:
        QByteArray m_keyAndIv;
    };

    QByteArray GenerateRandomSequence(size_t sequenceLen);
    inline uint64_t Rotate64(uint64_t value, int steps)
    {
      const unsigned int mask = (CHAR_BIT*sizeof(value) - 1);
      steps &= mask;
      return (value>>steps) | (value<<( (-steps)&mask ));
    }
}
