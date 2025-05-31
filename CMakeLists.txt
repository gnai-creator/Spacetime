cmake_minimum_required(VERSION 3.10)
project(ToroidEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(ENGINE_DIR ${SOURCE_DIR}/engine)

include_directories(
    ${SOURCE_DIR}
    ${ENGINE_DIR}
    /usr/include
    /usr/include/GL
    /usr/include/GLFW
    /usr/include/glm
)

file(GLOB_RECURSE SRC_FILES ${SOURCE_DIR}/*.cpp)

add_executable(ToroidEngine ${SRC_FILES})

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLEW REQUIRED glew)
pkg_search_module(GLFW REQUIRED glfw3)

include_directories(${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS})
target_link_libraries(ToroidEngine ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${OPENGL_gl_LIBRARY})

if(NOT GLEW_FOUND)
    target_link_libraries(ToroidEngine GLEW)
endif()

if(NOT GLFW_FOUND)
    target_link_libraries(ToroidEngine glfw)
endif()
