#pragma once

namespace passtore
{
    // Not thread safe
    // ! It is so strange, reimplement it !
    template<typename K, class T>
    class Cache
    {
    public:
        Cache(size_t capacity)
            : m_capacity(capacity)
        {
            assert(capacity > 0);
        }

        T* Get(const K& key)
        {
            if (!m_data.contains(key))
            {
                return nullptr;
            }

            m_lastUsed[QDateTime::currentMSecsSinceEpoch()] = key;

            return &m_data[key];
        }

        T& Set(const K& key, const T& obj)
        {
            auto& res = m_data[key] = obj;

            if (m_data.size() == m_capacity)
            {
                Remove(takeLastUsed());
            }

            return res;
        }

        void Remove(const K& key)
        {
            auto found = m_data.find(key);
            if (found != m_data.end())
            {
                m_data.erase(found);
            }
        }

    private:
        K takeLastUsed()
        {
            auto candidate = m_lastUsed.begin();
            if (candidate == m_lastUsed.end())
            {
                return m_data.begin().key();
            }
            return *candidate;
        }

    private:
        const size_t m_capacity;
        QMap<K, T> m_data;
        QMap<quint64, K> m_lastUsed;
    };
}
