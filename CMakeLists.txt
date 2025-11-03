cmake_minimum_required(VERSION 4.0)
project(projekt)

set(CMAKE_CXX_STANDARD 20)

# Add include directory
include_directories(include)

# Find all source files in the src directory
file(GLOB SOURCES "src/*.cpp")

# Add the executable with all the sources
add_executable(projekt ${SOURCES})

# Pass the project source dir to the code
target_compile_definitions(projekt PRIVATE PROJECT_SOURCE_DIR=${CMAKE_SOURCE_DIR})
