# CMake configuration for Passtore project

cmake_minimum_required(VERSION 3.10)
project(Passtore)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
	add_compile_options(/FS)
endif()

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable Visual Studio solution folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Recursively assign a FOLDER property to all targets under a directory tree
function(set_targets_folder dir folder)
    get_property(subdirs DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
    foreach(subdir IN LISTS subdirs)
        set_targets_folder("${subdir}" "${folder}")
    endforeach()
    get_property(targets DIRECTORY "${dir}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(target IN LISTS targets)
        set_property(TARGET "${target}" PROPERTY FOLDER "${folder}")
    endforeach()
endfunction()

# Add subdirectories
add_subdirectory(ext)
add_subdirectory(src)