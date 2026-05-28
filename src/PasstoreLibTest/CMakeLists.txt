# PasstoreLibTest CMakeLists.txt

# Define source files
set(PASSTORELIBTEST_SOURCES
    CryptorTest.cpp
    SQLiteDatabaseTest.cpp
    TableSettingsTest.cpp
    main.cpp
    pch.cpp
)

# Create the executable
add_executable(PasstoreLibTest
    ${PASSTORELIBTEST_SOURCES}
)

# Set precompiled header
set_target_properties(PasstoreLibTest PROPERTIES
    CXX_PCH_HEADER pch.h
    CXX_PCH_SOURCE pch.cpp
)

# Include directories
target_include_directories(PasstoreLibTest PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link dependencies
target_link_libraries(PasstoreLibTest
    PasstoreLib
    gtest
    sqlite
    TinyAES
)