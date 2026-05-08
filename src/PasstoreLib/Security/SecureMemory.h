#pragma once

#include <cstddef>
#include <cstdint>

#ifdef _WIN32
#include <windows.h>
#endif

namespace passtore
{
    inline void SecureWipe(void* ptr, size_t size) noexcept
    {
        if (ptr == nullptr || size == 0)
        {
            return;
        }

#ifdef _WIN32
        SecureZeroMemory(ptr, size);
#else
        volatile uint8_t* p = static_cast<volatile uint8_t*>(ptr);
        for (size_t i = 0; i < size; ++i)
        {
            p[i] = 0;
        }
#endif
    }

    inline bool TryLockMemory(void* ptr, size_t size) noexcept
    {
        if (ptr == nullptr || size == 0)
        {
            return false;
        }

#ifdef _WIN32
        return VirtualLock(ptr, size) != 0;
#else
        return false;
#endif
    }

    inline void UnlockMemory(void* ptr, size_t size) noexcept
    {
        if (ptr == nullptr || size == 0)
        {
            return;
        }

#ifdef _WIN32
        VirtualUnlock(ptr, size);
#endif
    }
}
