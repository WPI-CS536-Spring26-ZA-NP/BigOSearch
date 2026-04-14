cmake_minimum_required(VERSION 3.15)

project(bigo LANGUAGES CXX)

Set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE srcs src/*.c src/*.cpp)

add_library(${CMAKE_PROJECT_NAME} ${srcs})
target_include_directories(${PROJECT_NAME} PRIVATE "include")
