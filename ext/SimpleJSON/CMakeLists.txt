# SimpleJSON library CMakeLists.txt

add_library(SimpleJSON INTERFACE)

target_include_directories(SimpleJSON SYSTEM INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)