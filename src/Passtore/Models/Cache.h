#pragma once
#include <map>
#include <chrono>
#include <cassert>

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
            auto found = m_data.find(key);
            if (found == m_data.end())
            {
                return nullptr;
            }

            uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
            m_lastUsed[now] = key;

            return &found->second;
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
                return m_data.begin()->first;
            }
            K key = candidate->second;
            m_lastUsed.erase(candidate);
            return key;
        }

    private:
        const size_t m_capacity;
        std::map<K, T> m_data;
        std::map<uint64_t, K> m_lastUsed;
    };
}
