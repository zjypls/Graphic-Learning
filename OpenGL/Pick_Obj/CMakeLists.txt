cmake_minimum_required(VERSION 3.23)
project(Pick_GL)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/bin>)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
#set(CMAKE_BUILD_TYPE RELEASE)

include_directories(${Root_Inc}/Dep ${Root_Inc} ${Root_Inc}/glfw/include
        ${Root_Inc}/glm ${Root_Inc}/ImGui
        ${Root_Inc}/ImGui/backends
        dep)
link_directories(${CMAKE_SOURCE_DIR}/Lib)
add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC dep)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw3 ImGui Dep glad)
