cmake_minimum_required(VERSION 2.8)

project(miniSTL)
set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -lpthread")
add_executable(${PROJECT_NAME} "main.cpp")
target_sources(${PROJECT_NAME} PUBLIC
                                      "test/vector_test.cpp"
                                      "test/deque_test.cpp"
                                      "test/list_test.cpp"
                                      "test/forward_list_test.cpp"
                                      "test/array_test.cpp"
                                      "test/stack_test.cpp"
                                      "test/queue_test.cpp"
                                      "test/priority_queue_test.cpp"
                                      "test/set_test.cpp"
                                      "test/multiset_test.cpp"
                                      "test/map_test.cpp"
                                      "test/multimap_test.cpp"
                                        )
target_link_libraries(${PROJECT_NAME} gtest gtest_main)
