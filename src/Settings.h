#pragma once
#include "Resource.h"

struct ColumnSettings
{
    QString name;
    bool visible = false;
    bool blured = false;
};

struct TableSettings
{
    QMap<ResourceProperty, ColumnSettings> columns;
};
