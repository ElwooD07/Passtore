# External libraries CMakeLists.txt

# SQLite
add_subdirectory(sqlite)
configure_external_target(sqlite "ext")

# TinyAES
add_subdirectory(TinyAES)
configure_external_target(TinyAES "ext")

# gtest
add_subdirectory(gtest)
configure_external_target(gtest "ext")

# plog
add_subdirectory(plog)
configure_external_target(plog "ext")

# SimpleJSON
add_subdirectory(SimpleJSON)
configure_external_target(SimpleJSON "ext")

# FLTK — static, no demos/docs/fluid/tests
set(FLTK_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_FLUID OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_FLTK_OPTIONS OFF CACHE BOOL "" FORCE)
set(FLTK_USE_SYSTEM_LIBPNG OFF CACHE BOOL "" FORCE)
set(FLTK_USE_SYSTEM_LIBJPEG OFF CACHE BOOL "" FORCE)
set(FLTK_USE_SYSTEM_ZLIB OFF CACHE BOOL "" FORCE)
add_subdirectory(fltk)