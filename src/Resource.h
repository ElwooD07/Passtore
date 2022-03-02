#pragma once
#include <QString>
#include <QVector>

enum ResourceProperty
{
    ResourcePropertyUnknown = -1,
    ResourcePropertyName = 0,
    ResourcePropertyEmail,
    ResourcePropertyPassword,
    ResourcePropertyUsername,
    ResourcePropertyDescription,
    ResourcePropertyAdditional,
    ResourcePropertiesCount
};

class Resource
{
public:
    explicit Resource(const QString& name = "");

    QString Value(ResourceProperty type) const;
    void SetValue(ResourceProperty prop, const QString& value);
    static QString PropertyName(ResourceProperty prop);
    static QString PropertyNameTranslated(ResourceProperty prop);

private:
    QVector<QString> m_values;
};
