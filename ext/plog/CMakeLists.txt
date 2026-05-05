# plog library CMakeLists.txt

# plog is a header-only library, so we create an INTERFACE library
add_library(plog INTERFACE)

target_include_directories(plog INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)