# External libraries CMakeLists.txt

# SQLite
add_subdirectory(sqlite)
set_targets_folder("${CMAKE_CURRENT_SOURCE_DIR}/sqlite" "ext")

# TinyAES
add_subdirectory(TinyAES)
set_targets_folder("${CMAKE_CURRENT_SOURCE_DIR}/TinyAES" "ext")

# gtest
add_subdirectory(gtest)
set_targets_folder("${CMAKE_CURRENT_SOURCE_DIR}/gtest" "ext")

# plog
add_subdirectory(plog)
set_targets_folder("${CMAKE_CURRENT_SOURCE_DIR}/plog" "ext")

# SimpleJSON
add_subdirectory(SimpleJSON)
set_targets_folder("${CMAKE_CURRENT_SOURCE_DIR}/SimpleJSON" "ext")

# FLTK — static, no demos/docs/fluid/tests
set(FLTK_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_FLUID OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_FLTK_OPTIONS OFF CACHE BOOL "" FORCE)
set(FLTK_USE_SYSTEM_LIBPNG OFF CACHE BOOL "" FORCE)
set(FLTK_USE_SYSTEM_LIBJPEG OFF CACHE BOOL "" FORCE)
set(FLTK_USE_SYSTEM_ZLIB OFF CACHE BOOL "" FORCE)
add_subdirectory(fltk)
set_targets_folder("${CMAKE_CURRENT_SOURCE_DIR}/fltk" "ext/fltk")