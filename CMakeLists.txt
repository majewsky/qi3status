project(qi3status)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
cmake_policy(SET CMP0043 NEW)
find_package(Qt5 REQUIRED NO_MODULE COMPONENTS DBus Network)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(qi3status
    main.cpp
    network.cpp
    renderer.cpp
    upower.cpp
)

qt5_use_modules(qi3status Core DBus Network)
install(TARGETS qi3status DESTINATION bin)
