cmake_minimum_required(VERSION 3.10)

project(opengl_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add_definitions("-Wall")

# Find SDL3 using find_package
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(glbinding REQUIRED)

# Include directories from SDL3
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${SDL3_INCLUDE_DIRS}
)

# Source files
file(GLOB SOURCES "src/*.cpp")

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# make name main
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "main")

# Link against SDL3
target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    SDL3::SDL3
    glbinding::glbinding    
)