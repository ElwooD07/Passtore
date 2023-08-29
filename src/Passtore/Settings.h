#pragma once
#include <QVector>

namespace passtore
{
    struct ColumnSettings
    {
        QString name;
        bool visible = false;
        bool blured = false;
    };

    struct TableSettings
    {
        QVector<ColumnSettings> columns;
    };
}
