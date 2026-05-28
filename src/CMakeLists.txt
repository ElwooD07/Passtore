# Main src CMakeLists.txt

# Add subdirectories
add_subdirectory(PasstoreLib)
set_targets_folder("${CMAKE_CURRENT_SOURCE_DIR}/PasstoreLib" "src")

add_subdirectory(Passtore)
set_targets_folder("${CMAKE_CURRENT_SOURCE_DIR}/Passtore" "src")

add_subdirectory(PasstoreLibTest)
set_targets_folder("${CMAKE_CURRENT_SOURCE_DIR}/PasstoreLibTest" "src")