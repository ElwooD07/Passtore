# Passtore application CMakeLists.txt

set(PASSTORE_SOURCES
    Models/ResourceTableModel.cpp
    Widgets/MainWindow_nana.cpp
    Widgets/ResourcesTableWidget_nana.cpp
    Settings.cpp
    main.cpp
    pch.cpp
)

set(PASSTORE_HEADERS
    Models/Cache.h
    Models/ITableModel.h
    Models/ResourceTableModel.h
    Settings.h
    Widgets/MainWindow.h
    Widgets/ResourcesTableWidget.h
    pch.h
)

add_executable(Passtore
    ${PASSTORE_SOURCES}
    ${PASSTORE_HEADERS}
)

set_target_properties(Passtore PROPERTIES
    CXX_PCH_HEADER pch.h
    CXX_PCH_SOURCE pch.cpp
)

target_include_directories(Passtore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Passtore
    PasstoreLib
    TinyAES
    sqlite
    plog
    nana
    gdi32
    user32
    comctl32
    ole32
    uuid
    ws2_32
)