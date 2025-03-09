cmake_minimum_required(VERSION 3.10)
project(GUIConsole)

find_package(FLTK REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} fltk X11)