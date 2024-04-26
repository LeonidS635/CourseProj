cmake_minimum_required(VERSION 3.27)
project(grasp4)

set(Boost_ROOT "C:/Program Files/Boost/boost_1_84_0")
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(grasp4 main.cpp
        headers/structures.h
        headers/algorithm.h
        algorithm.cpp
        headers/graph.h
        graph.cpp
        headers/generator.h
        generator.cpp
        headers/validator.h
        validator.cpp
        headers/tester.h
        tester.cpp
)
target_link_libraries(grasp4 ${Boost_LIBRARIES})
