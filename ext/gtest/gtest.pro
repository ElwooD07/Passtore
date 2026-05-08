TEMPLATE = lib
CONFIG += staticlib c++17
CONFIG -= qt

HEADERS += \
    include/gtest/gtest-assertion-result.h \
    include/gtest/gtest-death-test.h \
    include/gtest/gtest-matchers.h \
    include/gtest/gtest-message.h \
    include/gtest/gtest-param-test.h \
    include/gtest/gtest-printers.h \
    include/gtest/gtest-spi.h \
    include/gtest/gtest-test-part.h \
    include/gtest/gtest-typed-test.h \
    include/gtest/gtest.h \
    include/gtest/gtest_pred_impl.h \
    include/gtest/gtest_prod.h \
    include/gtest/internal/gtest-death-test-internal.h \
    include/gtest/internal/gtest-filepath.h \
    include/gtest/internal/gtest-internal.h \
    include/gtest/internal/gtest-param-util.h \
    include/gtest/internal/gtest-port-arch.h \
    include/gtest/internal/gtest-port.h \
    include/gtest/internal/gtest-string.h \
    include/gtest/internal/gtest-type-util.h \
    src/gtest-internal-inl.h

SOURCES += \
    src/gtest-all.cc \
    src/gtest-assertion-result.cc \
    src/gtest-death-test.cc \
    src/gtest-filepath.cc \
    src/gtest-matchers.cc \
    src/gtest-port.cc \
    src/gtest-printers.cc \
    src/gtest-test-part.cc \
    src/gtest-typed-test.cc \
    src/gtest.cc

INCLUDEPATH += include
