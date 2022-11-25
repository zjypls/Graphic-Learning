cmake_minimum_required(VERSION 3.2)

project(ImGui)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Lib)
set(CMAKE_CXX_STANDARD 17 CONFIG REQUIRED)

file(GLOB libSRC config_depends *.c *.cpp *.h ../GLFW)
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(LIB_PATH ${CMAKE_SOURCE_DIR}/Lib/lib-mingw64)
ELSEIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(LIB_PATH ${CMAKE_SOURCE_DIR}/Lib/lib-vc2022)
ELSE()
    message(FATAL_ERROR "There is No libglfw3 support for ${CMAKE_CXX_COMPILER_ID}")
ENDIF()
link_directories(${LIB_PATH})
add_library(${PROJECT_NAME} ${libSRC})
target_link_libraries(${PROJECT_NAME} PUBLIC glfw3)

