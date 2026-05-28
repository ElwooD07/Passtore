# PasstoreLib CMakeLists.txt

# Define source files
set(PASSTORELIB_SOURCES
    Security/Cryptor.cpp
    Storages/Marshaling.cpp
    Storages/TableSettings.cpp
    Storages/SQLite/CellsCryptor.cpp
    Storages/SQLite/Connection.cpp
    Storages/SQLite/Query.cpp
    Storages/SQLite/SQLiteDatabase.cpp
    Storages/SQLite/Transaction.cpp
    Utils/DataUtils.cpp
    Utils/sha256.cpp
    pch.cpp
)

set(PASSTORELIB_HEADERS
    Security/Cryptor.h
    Security/SecureMemory.h
    Security/SensitiveData.h
    Storages/IResourceStorage.h
    Storages/Marshaling.h
    Storages/TableSettings.h
    Storages/Resource.h
    Storages/SQLite/CellsCryptor.h
    Storages/SQLite/Connection.h
    Storages/SQLite/Query.h
    Storages/SQLite/SQLiteDatabase.h
    Storages/SQLite/Transaction.h
    Utils/DataUtils.h
    Utils/sha256.h
    pch.h
)

# Create the library
add_library(PasstoreLib STATIC
    ${PASSTORELIB_SOURCES}
    ${PASSTORELIB_HEADERS}
)

# Set precompiled header
set_target_properties(PasstoreLib PROPERTIES
    CXX_PCH_HEADER pch.h
    CXX_PCH_SOURCE pch.cpp
)

# Include directories
target_include_directories(PasstoreLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link dependencies
target_link_libraries(PasstoreLib
    sqlite
    SimpleJSON
    plog
    TinyAES
)