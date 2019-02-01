#pragma once
#include <QString>
#include <QSqlDatabase>
#include "Cryptor.h"

class Database
{
public:
    Database();
    Database(const Database&) = delete;

    void Open(const QString& path, const QString& password);
    void ChangePassword(const QString& newPassword);

private:
    void CreateNewConnection(const QString& path, const QString& password);

private:
    QSqlDatabase m_db;
    Cryptor m_cryptor;
};
