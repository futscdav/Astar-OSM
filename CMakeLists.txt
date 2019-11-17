cmake_minimum_required(VERSION 3.4)
project(Astar)

include_directories(src)
file(GLOB SOURCES "src/*.cpp")

add_executable(Astar ${SOURCES})

set_property(TARGET Astar PROPERTY CXX_STANDARD 17)