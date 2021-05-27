cmake_minimum_required(VERSION 2.8)

project(miniSTL)
set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -lpthread")
add_executable(${PROJECT_NAME} "main.cpp")
target_sources(${PROJECT_NAME} PUBLIC "test/vector_test.cpp")
target_link_libraries(${PROJECT_NAME} gtest gtest_main)
