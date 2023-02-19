cmake_minimum_required(VERSION 3.23)
project(Graphics)

set(Root_Inc ${CMAKE_SOURCE_DIR}/Include)
add_definitions(-DRootPath="${CMAKE_SOURCE_DIR}")

add_subdirectory(Include)

add_subdirectory(OpenGL)
