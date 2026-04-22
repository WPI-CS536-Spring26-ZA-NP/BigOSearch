cmake_minimum_required(VERSION 3.15)
include(FetchContent)

project(bigo LANGUAGES CXX)

Set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


FetchContent_Declare(
    Eigen
    GIT_REPOSITORY "https://gitlab.com/libeigen/eigen.git"
    GIT_TAG "3.4.1"
)
set(EIGEN_BUILD_DOC OFF)
set(EIGEN_BUILD_TESTING OFF)
FetchContent_MakeAvailable(Eigen)
# find_package(Eigen3 REQUIRED NO_MODULE)

file(GLOB_RECURSE srcs src/*.c src/*.cpp)

add_library(${CMAKE_PROJECT_NAME} ${srcs})
target_include_directories(${PROJECT_NAME} PUBLIC "include")
target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen)


file(GLOB_RECURSE testsrcs tests/*.c tests/*.cpp)


add_executable(Tests ${testsrcs})
target_link_libraries(Tests PRIVATE ${CMAKE_PROJECT_NAME})

enable_testing()
add_test(NAME RunTests COMMAND Tests)