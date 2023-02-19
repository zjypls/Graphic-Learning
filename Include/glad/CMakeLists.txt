cmake_minimum_required(VERSION 3.23)

project(glad)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Lib)
include_directories(../ dep)
add_library(glad glad.c glad.h)
target_include_directories(${PROJECT_NAME} PUBLIC dep)