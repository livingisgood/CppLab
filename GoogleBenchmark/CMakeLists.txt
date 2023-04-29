#project(GoogleBenchmark)

add_subdirectory(Lib/benchmark)

include_directories(../Utils)
include_directories(Lib/benchmark/include)
include_directories(../DataStructure)

add_executable(RunBenchmark UtilsBenchmark/MemoryPoolBenchmark.cpp DataStructureBenchmark/SmallVectorBenchmark.cpp)

target_link_libraries(RunBenchmark CppLab)
target_link_libraries(RunBenchmark benchmark::benchmark)