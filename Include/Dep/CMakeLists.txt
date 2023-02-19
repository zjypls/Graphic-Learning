CMAKE_MINIMUM_REQUIRED(VERSION 3.23)

set(CMAKE_CXX_STANDARD 17)
project(Dep)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Lib)

file(GLOB src config_depends *.cpp *.h)
include_directories(../ ../glm/ dep)
add_library(Dep ${src})
target_include_directories(${PROJECT_NAME} PUBLIC dep)