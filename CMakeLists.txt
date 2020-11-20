cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 20)
project(monitor)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(monitor ${SOURCES})

set_property(TARGET monitor PROPERTY CXX_STANDARD 20)
target_link_libraries(monitor ${CURSES_LIBRARIES} stdc++fs -std=c++2a)

target_compile_options(monitor PRIVATE -Wall -Wextra -Werror)
