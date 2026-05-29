#pragma once
#include <vector>
#include <stdint.h>
#include <span>
#include <string_view>
#include "Security/SecureMemory.h"
#include "Utils/DataUtils.h"

namespace passtore
{
    /* Heap-allocated sensitive buffer that is securely wiped on destruction and move; non-copyable. */
    class SensitiveData
    {
    public:
        using DataType = std::vector<uint8_t>;

        SensitiveData() = default;

        explicit SensitiveData(Secret data)
        {
            Assign(data);
        }

        explicit SensitiveData(std::string_view text)
        {
            Assign(text);
        }

        SensitiveData(const SensitiveData&) = delete;
        SensitiveData& operator=(const SensitiveData&) = delete;

        SensitiveData(SensitiveData&& other) noexcept
            : m_data(std::move(other.m_data))
            , m_isLocked(other.m_isLocked)
        {
            other.m_isLocked = false;
        }

        SensitiveData& operator=(SensitiveData&& other) noexcept
        {
            if (this == &other)
            {
                return *this;
            }

            WipeAndRelease();
            m_data = std::move(other.m_data);
            m_isLocked = other.m_isLocked;
            other.m_isLocked = false;
            return *this;
        }

        ~SensitiveData()
        {
            WipeAndRelease();
        }

        void Assign(Secret data)
        {
            WipeAndRelease();
            m_data.assign(data.begin(), data.end());
            m_isLocked = TryLockMemory(m_data.data(), m_data.size());
        }

        void Assign(std::string_view text)
        {
            Assign(Secret(reinterpret_cast<const uint8_t*>(text.data()), text.size()));
        }

        void Assign(const uint8_t* data, size_t size)
        {
            Assign(Secret(data, size));
        }

        void Assign(const char* text)
        {
            Assign(std::string_view(text));
        }

        void Wipe() noexcept
        {
            SecureWipe(m_data.data(), m_data.size());
        }

        Secret View() const noexcept
        {
            return Secret(m_data.data(), m_data.size());
        }

        std::span<uint8_t> MutableView() noexcept
        {
            return std::span<uint8_t>(m_data.data(), m_data.size());
        }

        const DataType& AsVector() const noexcept
        {
            return m_data;
        }

    private:
        void WipeAndRelease() noexcept
        {
            Wipe();
            if (m_isLocked)
            {
                UnlockMemory(m_data.data(), m_data.size());
            }
            m_data.clear();
            m_data.shrink_to_fit();
            m_isLocked = false;
        }

    private:
        DataType m_data;
        bool m_isLocked = false;
    };
}
