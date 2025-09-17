cmake_minimum_required(VERSION 4.1)

project(engraVis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders/")
add_compile_definitions(PROJECT_DIR="${CMAKE_SOURCE_DIR}/")
add_compile_definitions(ENGRAVINGS_DIR="${CMAKE_SOURCE_DIR}/engravings/")

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

include_directories(include)
include_directories( ${OPENGL_INCLUDE_DIRS} )
include_directories(external/imgui)


add_executable(engraVis
    src/main.cpp
    src/shader.cpp
    src/model.cpp
    src/gui.cpp
    external/glad.c
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(engraVis glfw OpenGL::GL)