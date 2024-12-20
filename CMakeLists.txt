cmake_minimum_required(VERSION 3.10)

project(OpenGL_test)

include_directories(
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/inc/SDL3
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_definitions("-Wall")

file( GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(test.bin ${SRC_FILES})

target_link_libraries(test.bin PRIVATE "${CMAKE_SOURCE_DIR}/bin/libSDL3.a" pthread dl m)
