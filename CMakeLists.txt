cmake_minimum_required(VERSION 3.20)
project(my_app C)

# Set C standard and compile flags
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -I${PROJECT_SOURCE_DIR}/include")

# Find ncurses library
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Get all .c files in src/
file(GLOB SOURCES "src/*.c")

# Create executable
add_executable(app ${SOURCES})

# Link ncurses
target_link_libraries(app ${CURSES_LIBRARIES})