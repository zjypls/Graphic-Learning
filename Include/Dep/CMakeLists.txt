CMAKE_MINIMUM_REQUIRED(VERSION 3.23)


project(Dep)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Lib)

file(GLOB src config_depends *.cpp *.h)

add_library(Dep ${src})