cmake_minimum_required(VERSION 3.24)
project(CppLab)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_library(CppLab library.cpp DataStructure/MemoryUtility.h DataStructure/SmallVector.h Utils/MemoryPool.cpp Utils/MemoryPool.h DataStructure/ContainerInserter.h DataStructure/VecView.h BattleCore/BattleCoreUsage.h SmartPointer/SmartPointer.h SmartPointer/SmartPointer.h DataStructure/IntrusivieList.h Utils/Event.h)
add_subdirectory(GoogleTest)
add_subdirectory(GoogleBenchmark)