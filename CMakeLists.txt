# CMake configuration for Passtore project

cmake_minimum_required(VERSION 3.10)
project(Passtore)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
	add_compile_options(/FS)
    add_compile_options(/WX)
else()
    add_compile_options(-Werror)
endif()

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable Visual Studio solution folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

function(configure_internal_target target folder)
    set_property(TARGET "${target}" PROPERTY FOLDER "${folder}")

    if (MSVC)
        target_compile_options("${target}" PRIVATE /WX)
    else()
        target_compile_options("${target}" PRIVATE -Werror)
    endif()
endfunction()

function(configure_external_target target folder)
    set_property(TARGET "${target}" PROPERTY FOLDER "${folder}")

    get_target_property(targetType "${target}" TYPE)
    if (targetType STREQUAL "INTERFACE_LIBRARY")
        return()
    endif()

    if (MSVC)
        target_compile_options("${target}" PRIVATE /WX-)
    else()
        target_compile_options("${target}" PRIVATE -Wno-error)
    endif()
endfunction()

# Add subdirectories
add_subdirectory(ext)
add_subdirectory(src)