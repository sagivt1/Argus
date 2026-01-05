cmake_minimum_required(VERSION 3.25)
project(ArgusEngine LANGUAGES CXX)

# Enforce Modern C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(ArgusEngine src/main.cpp)

