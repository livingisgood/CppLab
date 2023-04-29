cmake_minimum_required(VERSION 3.24)
project(CppLab)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_library(CppLab library.cpp DataStructure/MemoryUtility.h DataStructure/SmallVector.h DataStructure/CircularQueue.h Utils/MemoryPool.cpp Utils/MemoryPool.h DataStructure/ContainerInserter.h DataStructure/VecView.h BattleCore/BattleCoreUsage.h Utils/Event.h Utils/Event.cpp Utils/Allocator.cpp Utils/Allocator.h)
add_subdirectory(GoogleTest)
add_subdirectory(GoogleBenchmark)