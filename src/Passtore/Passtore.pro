QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    Models/Cache.h \
    Models/ResourceTableModel.h \
    Models/ResourceTableModelRoles.h \
    Models/ResourceViewDelegate.h \
    Settings.h \
    Widgets/ColumnSettingsWidget.h \
    Widgets/MainWindow.h \
    Widgets/ResourcesListWidget.h \
    Widgets/SettingsDialog.h \
    pch.h

SOURCES += \
    Models/ResourceTableModel.cpp \
    Models/ResourceViewDelegate.cpp \
    Settings.cpp \
    Widgets/ColumnSettingsWidget.cpp \
    Widgets/MainWindow.cpp \
    Widgets/ResourcesListWidget.cpp \
    Widgets/SettingsDialog.cpp \
    main.cpp \
    pch.cpp

FORMS += \
    Ui/AddResourceDialog.ui \
    Ui/ColumnSettingsWidget.ui \
    Ui/MainWindow.ui \
    Ui/ResourcesListWidget.ui \
    Ui/SettingsDialog.ui

UI_DIR = Ui

CONFIG += precompile_header
PRECOMPILED_HEADER = pch.h
PRECOMPILED_SOURCE = pch.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../PasstoreLib/release/ -lPasstoreLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../PasstoreLib/debug/ -lPasstoreLib
else:unix: LIBS += -L$$OUT_PWD/../PasstoreLib/ -lPasstoreLib

INCLUDEPATH += $$PWD/../PasstoreLib
DEPENDPATH += $$PWD/../PasstoreLib

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
