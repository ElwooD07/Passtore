#include "stdafx.h"
#include "Resource.h"

namespace
{
    static const char* s_propNames[] = {
        QT_TR_NOOP("Resource"),
        QT_TR_NOOP("Email"),
        QT_TR_NOOP("Password"),
        QT_TR_NOOP("Username"),
        QT_TR_NOOP("Description"),
        QT_TR_NOOP("Additional"),
    };
    static_assert (sizeof(s_propNames)/ sizeof(const char*) == ResourcePropertiesCount,
                   "Not all names for resource properties are specified");
}

Resource::Resource(const QString& name)
{
    m_values.resize(ResourcePropertiesCount);
    m_values[ResourcePropertyName] = name;
}

QString Resource::Value(ResourceProperty prop) const
{
    return m_values.at(prop);
}

void Resource::SetValue(ResourceProperty prop, const QString& value)
{
    m_values[prop] = value;
}

QString Resource::PropertyName(ResourceProperty prop)
{
    return s_propNames[prop];
}

QString Resource::PropertyNameTranslated(ResourceProperty prop)
{
    return QObject::tr(s_propNames[prop]);
}
