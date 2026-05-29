# Passtore application CMakeLists.txt

set(PASSTORE_SOURCES
    Models/ResourceTableModel.cpp
    Settings.cpp
    Widgets/ChangePasswordDialog.cpp
    Widgets/ColumnSettingsWidget.cpp
    Widgets/MainWindow.cpp
    Widgets/ResourcesTableWidget.cpp
    Widgets/SettingsDialog.cpp
    main.cpp
    pch.cpp
)

set(PASSTORE_HEADERS
    Models/Cache.h
    Models/ITableModel.h
    Models/ResourceTableModel.h
    Models/ResourceTableModelRoles.h
    Settings.h
    Widgets/ChangePasswordDialog.h
    Widgets/ColumnSettingsWidget.h
    Widgets/MainWindow.h
    Widgets/ResourcesTableWidget.h
    Widgets/SettingsDialog.h
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
    fltk
    gdi32
    user32
    comctl32
    ole32
    uuid
    ws2_32
)