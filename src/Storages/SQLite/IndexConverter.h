#pragma once

namespace passtore
{
    class IndexConverter
    {
    public:
        size_t Count();
        size_t ToRowId(size_t dbId);
        size_t ToId(size_t rowId);
        size_t AddRowId(size_t rowId); // Returns Id
        void RemoveId(size_t dbId);

        bool isEmpty();
        static constexpr auto InvalidId = static_cast<size_t>(-1);

    private:
        QVector<size_t>& GetRowIds();

    private:
        QVector<size_t> m_rowIds;
    };
}
