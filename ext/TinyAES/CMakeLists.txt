# TinyAES library CMakeLists.txt

# Add the source files
add_library(TinyAES STATIC
    aes.c
    aes.h
)

# Set include directory
target_include_directories(TinyAES SYSTEM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)