#include "pch.h"
#include "Database.h"
#include "DatabaseQueries.h"

namespace
{
    const std::string s_phraseDecryptedExpected = PRODUCT_NAME + std::to_string(VER_MAJOR);
    QByteArray GenerateRandomSequence()
    {
        size_t sequenceLen = 128;
        QByteArray result;
        srand(QDateTime::currentDateTimeUtc().toSecsSinceEpoch());
        while (sequenceLen--)
        {
            result.append(static_cast<char>(rand() % 255));
        }
        return result;
    }
}

Database::Database()
    : m_db(QSqlDatabase::addDatabase("QSQLITE"))
{ }

void Database::Open(const QString& path, const QString& password)
{
    CreateNewConnection(path, password);
    // Uncomment it for tests when the database is empty
    /*Resource res;
    res.SetValue(ResourcePropertyResource, "ResName");
    res.SetValue(ResourcePropertyPassword, "arbadakarba");
    res.SetValue(ResourcePropertyDescription, "bla-bla-bla");
    int id = AddResource(res);
    qDebug() << id;*/

    // TODO : Collect data from DB
}

void Database::ChangePassword(const QString& newPassword)
{
    QByteArray passwordHash = QCryptographicHash::hash(newPassword.toUtf8(), QCryptographicHash::Sha256);
    Cryptor passwordCryptor(passwordHash);

    QByteArray originalKeyAndIv = m_cryptor.Key() + m_cryptor.Iv();
    QByteArray newPhrase = passwordCryptor.Encrypt(QString(s_phraseDecryptedExpected.c_str()).toUtf8());
    QByteArray newKeys = passwordCryptor.Encrypt(originalKeyAndIv);

    if (!m_db.transaction())
    {
        throw std::runtime_error(QObject::tr("Unable to begin transaction: %1").arg(m_db.lastError().text()).toStdString());
    }

    // Delete old Metadata table and create new one
    QSqlQuery query;
    if (!query.exec("DROP TABLE IF EXISTS Metadata;") ||
        !query.exec("CREATE TABLE Metadata ('phrase' BLOB, 'keys' BLOB);"))
    {
        throw std::runtime_error(QObject::tr("Unable to reset Metadata table: %1").arg(query.lastError().text()).toStdString());
    }

    query.prepare("INSERT INTO Metadata ('phrase', 'keys') VALUES (?, ?);");
    query.addBindValue(newPhrase);
    query.addBindValue(newKeys);
    if (!query.exec())
    {
        throw std::runtime_error(QObject::tr("Unable to shange password: %1").arg(query.lastError().text()).toStdString());
    }

    if (!m_db.commit())
    {
        throw std::runtime_error(QObject::tr("Unable to commit changes to the database").toStdString());
    }
}

ResourceDefinitions Database::GetResourceDefinitions()
{
    QSqlQuery query;
    if (!query.exec(MakeResourceSelectDefinitionsQuery()))
    {
        throw std::runtime_error(QObject::tr("Unable to get data from the database: %1").arg(query.lastError().text()).toStdString());
    }

    ResourceDefinitions result;
    if (query.first())
    {
        do {
            result.push_back( { query.value(0).toInt(), m_cryptor.DecryptAsString(query.value(1).toByteArray()) } );
        } while(query.next());
    }
    return result;
}

Resource Database::GetResource(int id)
{
    QSqlQuery query;
    if (!query.exec(MakeResourceSelectQuery(id)) || !query.first())
    {
        throw std::runtime_error(QObject::tr("Unable to get resource record from the database: %1").arg(query.lastError().text()).toStdString());
    }

    Resource result;
    for (int i = ResourcePropertyName; i < ResourcePropertyCount; ++i)
    {
        result.SetValue(static_cast<ResourceProperty>(i), m_cryptor.DecryptAsString(query.value(i).toByteArray()));
    }
    return result;
}

QString Database::GetResourcePropertyValue(int id, ResourceProperty prop)
{
    QSqlQuery query;
    if (!query.exec(MakeResourceSelectPropertyQuery(id, prop)) || !query.first())
    {
        throw std::runtime_error(QObject::tr("Unable to get resource record from the database: %1").arg(query.lastError().text()).toStdString());
    }
    return m_cryptor.DecryptAsString(query.value(0).toByteArray());
}

void Database::SetResource(int id, const Resource& resource)
{
    QSqlQuery query;
    query.prepare(MakeResourceUpdateQuery(id));
    for (int i = ResourcePropertyName; i < ResourcePropertyCount; ++i)
    {
        query.addBindValue(m_cryptor.Encrypt(resource.Value(static_cast<ResourceProperty>(i))));
    }

    if (!query.exec())
    {
        throw std::runtime_error(QObject::tr("Unable to write resource to the database: %1").arg(query.lastError().text()).toStdString());
    }
}

void Database::SetResourcePropertyValue(int id, ResourceProperty prop, const QString& value)
{
    QSqlQuery query;
    query.prepare(MakeResourcePropertyUpdateQuery(id, prop));
    query.addBindValue(m_cryptor.Encrypt(value), QSql::In | QSql::Binary);
    if (!query.exec())
    {
        throw std::runtime_error(QObject::tr("Unable to write resource property to the database: %1").arg(query.lastError().text()).toStdString());
    }
}

int Database::CreateResource()
{
    QSqlQuery query;
    query.prepare(MakeResourceInsertQuery());

    if (!query.exec())
    {
        throw std::runtime_error(QObject::tr("Unable to write resource to the database: %1").arg(query.lastError().text()).toStdString());
    }
    return query.lastInsertId().toInt();
}

int Database::AddResource(const Resource& resource)
{
    QSqlQuery query;
    query.prepare(MakeResourceInsertQueryValues());
    for (int i = ResourcePropertyName; i < ResourcePropertyCount; ++i)
    {
        query.addBindValue(m_cryptor.Encrypt(resource.Value(static_cast<ResourceProperty>(i))));
    }

    if (!query.exec())
    {
        throw std::runtime_error(QObject::tr("Unable to write resource to the database: %1").arg(query.lastError().text()).toStdString());
    }
    return query.lastInsertId().toInt();
}

void Database::CreateNewConnection(const QString& path, const QString& password)
{
    if (m_db.isOpen())
    {
        m_db.close();
    }

    m_db.setDatabaseName(path);
    if (!m_db.open())
    {
        throw std::runtime_error(QObject::tr("Unable to open database: %1").arg(m_db.lastError().text()).toStdString());
    }

    QSqlQuery query;
    if (query.exec("SELECT count(*), phrase, keys FROM Metadata;") &&
        query.first())
    {
        // Existing proper database is opened

        if (query.value(0).toInt() != 1)
        {
            throw std::runtime_error(QObject::tr("The database has wrong scheme or corrupted").toStdString());
        }
        QByteArray phrase = query.value(1).toByteArray();
        QByteArray encryptedKey = query.value(2).toByteArray();

        Cryptor passwordCryptor(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256));
        std::string phraseDecrypted = passwordCryptor.DecryptAsString(phrase).toUtf8().constData();
        if (phraseDecrypted != s_phraseDecryptedExpected)
        {
            throw std::runtime_error("Invalid password");
        }
        m_cryptor.SetKeys(passwordCryptor.Decrypt(encryptedKey));
    }
    else
    {
        // New database is created or not valid database is opened

        if (!query.exec("SELECT count(*) FROM sqlite_master WHERE type='table';") ||
            !query.first())
        {
            throw std::runtime_error(QObject::tr("Wrong database is opened: %1").arg(query.lastError().text()).toStdString());
        }

        bool ok = false;
        int existingTables = query.value(0).toInt(&ok);
        if (!ok || existingTables > 0)
        {
            throw std::runtime_error(QObject::tr("Wrong database is opened").toStdString());
        }

        // It is really empty database, so create tables
        QByteArray keyAndIv = QCryptographicHash::hash(GenerateRandomSequence(), QCryptographicHash::Sha256);
        m_cryptor.SetKeys(keyAndIv);

        ChangePassword(password);

        if (!query.exec(MakeResourcesTableCreateQuery()))
        {
            throw std::runtime_error(QObject::tr("Unable to create database structure: %1").arg(query.lastError().text()).toStdString());
        }
    }
}
