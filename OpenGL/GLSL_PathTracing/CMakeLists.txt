cmake_minimum_required(VERSION 3.23)
project(GLSL_PathTracing)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE RELEASE)
message(${CMAKE_SOURCE_DIR})
#set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}\\bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/bin>)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
file(GLOB src CONFIG_DEPENDS *.cpp glad.c *.h)
include_directories(${CMAKE_SOURCE_DIR}/Include/OpenGL dep)
include_directories(${CMAKE_SOURCE_DIR}/Include/ImGui dep)

link_directories(${CMAKE_SOURCE_DIR}/Include/OpenGL/lib-mingw64)

add_executable(GLSL_PathTracing)
target_include_directories(GLSL_PathTracing PUBLIC dep)
target_link_libraries(GLSL_PathTracing PUBLIC glfw3)

target_sources(GLSL_PathTracing PUBLIC ${src})