#pragma once
#include <cstdint>

namespace passtore
{
    enum ResourceTableModelRole: uint32_t
    {
        First = 1000,   // equivalent of Qt::UserRole
        IsBigColumn = First,
    };
}
