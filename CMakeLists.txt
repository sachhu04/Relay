cmake_minimum_required(VERSION 3.14)
project(Relay VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Setup basic structure
add_executable(relay src/main.cpp)
