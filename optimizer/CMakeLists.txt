cmake_minimum_required(VERSION 3.16)
project(iMOPSE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")

file(GLOB_RECURSE SOURCES "src/*.h" "src/*.cpp")

include_directories(src)
add_executable(imopse ${SOURCES})
