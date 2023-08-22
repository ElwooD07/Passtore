#pragma once
#include <vector>
#include <stdint.h>

namespace passtore
{
    class SensitiveData
    {
    public:
        using DataType = std::vector<uint8_t>;

        template<class T>
        SensitiveData(T&& data)
        {
            Take(std::move(data));
        }

        ~SensitiveData()
        {
            ClearContainer(m_data);
        }

        void Take(DataType&& data)
        {
            m_data.assign(data.begin(), data.end());
            ClearContainer(data);
        }

        void Take(std::string&& text)
        {
            m_data.assign(text.begin(), text.end());
            ClearContainer(text);
        }

        void Take(const uint8_t* data, size_t size)
        {
            m_data.assign(data, data + size);
        }

        void Take(const char* text)
        {
            m_data.assign(text, text + strlen(text));
        }

#ifdef USE_QT
        void Take(QString&& text)
        {
            auto utf8(text.toUtf8());
            m_data.assign(utf8.begin(), utf8.end());

            ClearContainer(utf8);
            ClearContainer(text);
        }

        void Take(QByteArray&& data)
        {
            m_data.assign(data.begin(), data.end());
            ClearContainer(data);
        }

        QByteArrayView AsByteArray() const
        {
            return QByteArrayView(m_data.data(), m_data.size());
        }
#endif

        template<class Container>
        static void ClearContainer(Container& cont)
        {
            memset(cont.data(), 0, cont.size());
        }

        const DataType& operator()() const
        {
            return m_data;
        }

        const DataType& AsVector() const
        {
            return m_data;
        }

    private:
        DataType m_data;
    };
}
