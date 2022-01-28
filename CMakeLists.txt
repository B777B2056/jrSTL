cmake_minimum_required(VERSION 2.8)

project(jrSTL)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(test TEST_LIST)
add_executable(${PROJECT_NAME} "main.cpp" ${TEST_LIST})

target_link_libraries(${PROJECT_NAME} /usr/local/lib/libgtest.a /usr/local/lib/libgtest_main.a pthread)
