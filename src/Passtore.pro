#-------------------------------------------------
#
# Project created by QtCreator 2019-01-12T14:21:42
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Passtore
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
    Models/ResourceTableModel.cpp \
    Models/ResourceViewDelegate.cpp \
    Settings.cpp \
    Storages/SQLite/IndexConverter.cpp \
    Storages/SQLite/SQLiteColumns.cpp \
    Storages/SQLite/SQLiteDatabase.cpp \
    Storages/SQLite/SQLiteDatabaseQueries.cpp \
    Widgets/ColumnSettingsWidget.cpp \
    Widgets/SettingsDialog.cpp \
    main.cpp \
    Widgets/MainWindow.cpp \
    Widgets/ResourcesListWidget.cpp \
    Cryptor.cpp \
    pch.cpp

HEADERS += \
    Models/ResourceTableModel.h \
    Models/ResourceTableModelRoles.h \
    Models/ResourceViewDelegate.h \
    Settings.h \
    Storages/IStorage.h \
    Storages/SQLite/IndexConverter.h \
    Storages/SQLite/SQLiteColumns.h \
    Storages/SQLite/SQLiteDatabase.h \
    Storages/SQLite/SQLiteDatabaseQueries.h \
    Widgets/ColumnSettingsWidget.h \
    Widgets/MainWindow.h \
    Widgets/ResourcesListWidget.h \
    ../3rd-party/TinyAES/aes.hpp \
    Cryptor.h \
    Widgets/SettingsDialog.h \
    pch.h

UI_DIR = Ui
FORMS += \
    Ui/ColumnSettingsWidget.ui \
    Ui/MainWindow.ui \
    Ui/ResourcesListWidget.ui \
    Ui/SettingsDialog.ui

INCLUDEPATH += ../3rd-party/TinyAES
INCLUDEPATH += ../3rd-party\gtest\googletest\include

CONFIG += precompile_header
PRECOMPILED_HEADER = pch.h
PRECOMPILED_SOURCE = pch.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
