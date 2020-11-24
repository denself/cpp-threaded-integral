cmake_minimum_required(VERSION 3.17)
project(ThreadedIntegral)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(ThreadedIntegral main.cpp functions.cpp functions.h integral.cpp integral.h timeit.h)