#pragma once
#include <list>
#include <unordered_map>
#include <cassert>

namespace passtore
{
    /* Fixed-capacity LRU cache. Not thread-safe. */
    template<typename K, class T>
    class Cache
    {
    public:
        explicit Cache(size_t capacity)
            : m_capacity(capacity)
        {
            assert(capacity > 0);
        }

        T* Get(const K& key)
        {
            auto it = m_map.find(key);
            if (it == m_map.end())
            {
                return nullptr;
            }
            m_order.splice(m_order.begin(), m_order, it->second.orderIt);
            return &it->second.value;
        }

        T& Set(const K& key, const T& obj)
        {
            auto it = m_map.find(key);
            if (it != m_map.end())
            {
                it->second.value = obj;
                m_order.splice(m_order.begin(), m_order, it->second.orderIt);
                return it->second.value;
            }

            if (m_map.size() == m_capacity)
            {
                m_map.erase(m_order.back());
                m_order.pop_back();
            }

            m_order.push_front(key);
            auto& entry = m_map[key];
            entry.value = obj;
            entry.orderIt = m_order.begin();
            return entry.value;
        }

        void Remove(const K& key)
        {
            auto it = m_map.find(key);
            if (it != m_map.end())
            {
                m_order.erase(it->second.orderIt);
                m_map.erase(it);
            }
        }

        void Clear()
        {
            m_map.clear();
            m_order.clear();
        }

    private:
        struct Entry
        {
            T value;
            typename std::list<K>::iterator orderIt;
        };

        const size_t m_capacity;
        std::list<K> m_order;
        std::unordered_map<K, Entry> m_map;
    };
}
