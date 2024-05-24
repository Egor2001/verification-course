cmake_minimum_required(VERSION 3.14)

project(project LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS -fsanitize=undefined)
set(CMAKE_CXX_FLAGS --coverage)
set(CMAKE_CXX_CPPCHECK "cppcheck")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()
find_package(GTest REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main GTest::GTest GTest::Main)
