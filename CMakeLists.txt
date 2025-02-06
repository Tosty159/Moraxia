cmake_minimum_required(VERSION 3.10)

project(Morx C)

set(CMAKE_C_STANDARD 99)

file(GLOB SOURCES "src/*.c")

add_executable(morx ${SOURCES})

install(TARGETS morx DESTINATION /usr/local/bin)