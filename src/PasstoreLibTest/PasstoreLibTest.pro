TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
QT     += core

SOURCES += \
        CryptorTest.cpp \
        SQLiteDatabaseTest.cpp \
        main.cpp \
        pch.cpp

CONFIG += precompile_header
PRECOMPILED_HEADER = pch.h
PRECOMPILED_SOURCE = pch.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../ext/gtest/release/ -lgtest
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../ext/gtest/debug/ -lgtest
else:unix: LIBS += -L$$OUT_PWD/../../ext/gtest/ -lgtest

INCLUDEPATH += $$PWD/../../ext/gtest/include
DEPENDPATH += $$PWD/../../ext/gtest

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../ext/gtest/release/libgtest.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../ext/gtest/debug/libgtest.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../ext/gtest/release/gtest.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../ext/gtest/debug/gtest.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../ext/gtest/libgtest.a

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../PasstoreLib/release/ -lPasstoreLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../PasstoreLib/debug/ -lPasstoreLib
else:unix: LIBS += -L$$OUT_PWD/../PasstoreLib/ -lPasstoreLib

INCLUDEPATH += $$PWD/../PasstoreLib
DEPENDPATH += $$PWD/../PasstoreLib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PasstoreLib/release/libPasstoreLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PasstoreLib/debug/libPasstoreLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PasstoreLib/release/PasstoreLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PasstoreLib/debug/PasstoreLib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../PasstoreLib/libPasstoreLib.a

HEADERS += \
    pch.h
