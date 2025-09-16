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


add_executable(engraVis
    src/main.cpp
    src/shader.cpp
    src/model.cpp
    external/glad.c
)

target_link_libraries(engraVis glfw OpenGL::GL)