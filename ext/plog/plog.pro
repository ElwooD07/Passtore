TEMPLATE = lib
CONFIG += staticlib c++17
CONFIG -= qt

HEADERS += \
    include/plog/Appenders/AndroidAppender.h \
    include/plog/Appenders/ColorConsoleAppender.h \
    include/plog/Appenders/ConsoleAppender.h \
    include/plog/Appenders/DebugOutputAppender.h \
    include/plog/Appenders/DynamicAppender.h \
    include/plog/Appenders/EventLogAppender.h \
    include/plog/Appenders/IAppender.h \
    include/plog/Appenders/RollingFileAppender.h \
    include/plog/Converters/NativeEOLConverter.h \
    include/plog/Converters/UTF8Converter.h \
    include/plog/Formatters/CsvFormatter.h \
    include/plog/Formatters/FuncMessageFormatter.h \
    include/plog/Formatters/MessageOnlyFormatter.h \
    include/plog/Formatters/TxtFormatter.h \
    include/plog/Helpers/AscDump.h \
    include/plog/Helpers/HexDump.h \
    include/plog/Helpers/PrintVar.h \
    include/plog/Init.h \
    include/plog/Initializers/ConsoleInitializer.h \
    include/plog/Initializers/RollingFileInitializer.h \
    include/plog/Log.h \
    include/plog/Logger.h \
    include/plog/Record.h \
    include/plog/Severity.h \
    include/plog/Util.h \
    include/plog/WinApi.h

INCLUDEPATH += include
