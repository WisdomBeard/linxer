cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

find_package(linxer CONFIG REQUIRED)

# set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)
add_executable(example src/example.cpp)
target_link_libraries(example linxer::linxer)
