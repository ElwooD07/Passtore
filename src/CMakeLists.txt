# Main src CMakeLists.txt

# Add subdirectories
add_subdirectory(PasstoreLib)
configure_internal_target(PasstoreLib "src")

add_subdirectory(Passtore)
configure_internal_target(Passtore "src")

add_subdirectory(PasstoreLibTest)
configure_internal_target(PasstoreLibTest "src")