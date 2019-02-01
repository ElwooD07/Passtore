#pragma once
#include <QByteArray>

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

private:
    void CheckKeys(const QByteArray& key, const QByteArray& iv) const;
    void CheckKeys() const;

private:
    QByteArray m_key;
    QByteArray m_iv;
};
