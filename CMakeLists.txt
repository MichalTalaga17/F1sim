cmake_minimum_required(VERSION 4.0)
project(projekt)

set(CMAKE_CXX_STANDARD 20)

# Add include directory
include_directories(include)

# List all source files explicitly
set(SOURCES
    src/main.cpp
    src/Car.cpp
    src/Race.cpp
    src/Track.cpp
    src/Weather.cpp
    src/Team.cpp
    src/Driver.cpp
    src/Agent.cpp
    src/Strategy.cpp
    src/Tire.cpp
    src/ConfigParser.cpp
)

# Add the executable with all the sources
add_executable(projekt ${SOURCES})

# Pass the project source dir to the code
target_compile_definitions(projekt PRIVATE PROJECT_SOURCE_DIR=${CMAKE_SOURCE_DIR})
