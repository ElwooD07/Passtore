#-------------------------------------------------
#
# Project created by QtCreator 2019-01-12T14:21:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = PasstoreLib
TEMPLATE = lib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += staticlib c++17

SOURCES += \
    Security/Cryptor.cpp \
    Storages/Marshaling.cpp \
    Storages/SQLite/CellsCryptor.cpp \
    Storages/SQLite/Connection.cpp \
    Storages/SQLite/Query.cpp \
    Storages/SQLite/SQLiteDatabase.cpp \
    Storages/SQLite/Transaction.cpp \
    Utils/DataUtils.cpp \
    Utils/sha256.cpp \
    pch.cpp

HEADERS += \
    Security/Cryptor.h \
    Security/SensitiveData.h \
    Storages/IResourceStorage.h \
    Storages/Marshaling.h \
    Storages/Resource.h \
    Storages/SQLite/CellsCryptor.h \
    Storages/SQLite/Connection.h \
    Storages/SQLite/Query.h \
    Storages/SQLite/SQLiteDatabase.h \
    Storages/SQLite/Transaction.h \
    Utils/DataUtils.h \
    Utils/sha256.h \
    pch.h

CONFIG += precompile_header
PRECOMPILED_HEADER = pch.h
PRECOMPILED_SOURCE = pch.cpp

INCLUDEPATH += $$PWD/../../ext/SimpleJSON

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../ext/sqlite/release/ -lsqlite
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../ext/sqlite/debug/ -lsqlite
else:unix: LIBS += -L$$OUT_PWD/../../ext/sqlite/ -lsqlite

INCLUDEPATH += $$PWD/../../ext/sqlite
DEPENDPATH += $$PWD/../../ext/sqlite

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../ext/sqlite/release/libsqlite.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../ext/sqlite/debug/libsqlite.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../ext/sqlite/release/sqlite.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../ext/sqlite/debug/sqlite.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../ext/sqlite/libsqlite.a

INCLUDEPATH += $$PWD/../../ext/plog/include

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../ext/TinyAES/release/ -lTinyAES
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../ext/TinyAES/debug/ -lTinyAES
else:unix: LIBS += -L$$OUT_PWD/../../ext/TinyAES/ -lTinyAES

INCLUDEPATH += $$PWD/../../ext/TinyAES
DEPENDPATH += $$PWD/../../ext/TinyAES

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../ext/TinyAES/release/libTinyAES.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../ext/TinyAES/debug/libTinyAES.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../ext/TinyAES/release/TinyAES.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../ext/TinyAES/debug/TinyAES.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../ext/TinyAES/libTinyAES.a
