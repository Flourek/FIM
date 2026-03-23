cmake_minimum_required(VERSION 3.20)
project(my_app C)

# Set C standard and compile flags
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -I${PROJECT_SOURCE_DIR}/include")

# Find ncurses library (prefer wide version)
find_path(NCURSESW_INCLUDE_DIR
  NAMES ncurses.h
  PATH_SUFFIXES ncursesw
)
find_library(NCURSESW_LIBRARY NAMES ncursesw)

if(NCURSESW_INCLUDE_DIR AND NCURSESW_LIBRARY)
    include_directories(${NCURSESW_INCLUDE_DIR})
    set(CURSES_LIBRARIES ${NCURSESW_LIBRARY})
else()
    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIR})
endif()

# Get all .c files in src/ and auto-reconfigure when files are added/removed
file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.c")

# Create executable
add_executable(app ${SOURCES})

# Link ncurses
target_link_libraries(app ${CURSES_LIBRARIES})