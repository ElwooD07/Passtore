#pragma once
#include "Storages/SQLite/Connection.h"

namespace passtore
{
    namespace sqlite
    {
        // TODO: Introduce ResourceId instead of it
        // TODO: TEST IT!
        class IndexConverter
        {
        public:
            explicit IndexConverter(Connection& connection);

            int Count();
            int ToRowId(int dbId);
            int ToId(int rowId);
            int AddRowId(int rowId); // Returns Id
            void RemoveId(int dbId);

            bool isEmpty();
            static constexpr int InvalidId = -1;

        private:
            QVector<size_t>& GetRowIds();

        private:
            QVector<size_t> m_rowIds;
            Connection& m_connection;
        };
    }
}
