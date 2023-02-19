cmake_minimum_required(VERSION 3.23)



project(ImGui)
add_subdirectory(Dep)
add_subdirectory(glad)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Lib)
set(CMAKE_CXX_STANDARD 17)
file(GLOB src CONFIG_DEPENDS ./ImGui/*.cpp ./ImGui/*.h
        ./ImGui/backends/imgui_impl_opengl3.cpp
        ./ImGui/backends/imgui_impl_glfw.cpp)
include_directories(./ImGui/backends ./GLFW/include ./ImGui dep)
link_directories(${Root_Inc}/Include/GLFW/src)

add_library(ImGui)
target_include_directories(${PROJECT_NAME} PUBLIC dep)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw3)
target_sources(${PROJECT_NAME} PUBLIC ${src})
