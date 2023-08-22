TEMPLATE = lib
CONFIG += staticlib c++17
CONFIG -= qt

HEADERS += \
    sqlite3.h \
    sqlite3ext.h

SOURCES += \
    sqlite3.c
