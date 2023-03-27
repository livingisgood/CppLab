cmake_minimum_required(VERSION 3.24)
project(CppLab)

set(CMAKE_CXX_STANDARD 14)

add_library(CppLab library.cpp DataStructure/MemoryUtility.h DataStructure/SmallVector.h DataStructure/CircularQueue.h)
add_subdirectory(GoogleTest)