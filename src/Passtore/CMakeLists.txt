# Passtore application CMakeLists.txt

# Define source files
set(PASSTORE_SOURCES
    Models/ResourceTableModel.cpp
    Models/ResourceViewDelegate.cpp
    Settings.cpp
    Widgets/ColumnSettingsWidget.cpp
    Widgets/MainWindow.cpp
    Widgets/ResourcesListWidget.cpp
    Widgets/SettingsDialog.cpp
    main.cpp
    pch.cpp
)

set(PASSTORE_HEADERS
    Models/Cache.h
    Models/ResourceTableModel.h
    Models/ResourceTableModelRoles.h
    Models/ResourceViewDelegate.h
    Settings.h
    Widgets/ColumnSettingsWidget.h
    Widgets/MainWindow.h
    Widgets/ResourcesListWidget.h
    Widgets/SettingsDialog.h
    pch.h
)

set(PASSTORE_UI_FILES
    Ui/ColumnSettingsWidget.ui
    Ui/MainWindow.ui
    Ui/ResourcesListWidget.ui
    Ui/SettingsDialog.ui
    Ui/AddResourceDialog.ui
)

# Create the executable
add_executable(Passtore
    ${PASSTORE_SOURCES}
    ${PASSTORE_HEADERS}
    ${PASSTORE_UI_FILES}
)

# Tell AUTOUIC where to find the .ui files
set_target_properties(Passtore PROPERTIES
    AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/Ui"
)

# Set precompiled header
set_target_properties(Passtore PROPERTIES
    CXX_PCH_HEADER pch.h
    CXX_PCH_SOURCE pch.cpp
)

# Include directories
target_include_directories(Passtore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link dependencies
target_link_libraries(Passtore
    PasstoreLib
    TinyAES
    sqlite
    plog
    Qt6::Core
    Qt6::Widgets
)