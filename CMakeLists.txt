cmake_minimum_required(VERSION 2.8)

project(miniSTL)
set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -pthread")
add_executable(${PROJECT_NAME} "main.cpp")
