# External libraries CMakeLists.txt

# SQLite
add_subdirectory(sqlite)

# TinyAES
add_subdirectory(TinyAES)

# gtest
add_subdirectory(gtest)

# plog
add_subdirectory(plog)

# SimpleJSON
add_subdirectory(SimpleJSON)

# FLTK — static, no demos/docs/fluid/tests
set(FLTK_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_FLUID OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_FLTK_OPTIONS OFF CACHE BOOL "" FORCE)
set(FLTK_USE_SYSTEM_LIBPNG OFF CACHE BOOL "" FORCE)
set(FLTK_USE_SYSTEM_LIBJPEG OFF CACHE BOOL "" FORCE)
set(FLTK_USE_SYSTEM_ZLIB OFF CACHE BOOL "" FORCE)
add_subdirectory(fltk)