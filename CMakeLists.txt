cmake_minimum_required(VERSION 3.0)

project(op)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

add_library(op parser.cpp)
target_include_directories(op PUBLIC include)

add_subdirectory(examples)
