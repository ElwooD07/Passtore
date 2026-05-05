# gtest library CMakeLists.txt
# gtest-all.cc already includes all other source files internally

add_library(gtest STATIC
    src/gtest-all.cc
)

target_include_directories(gtest PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)