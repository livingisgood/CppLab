project(GoogleTest)

add_subdirectory(Lib)
include_directories(${gtest_source_dir}/include ${gtest_source_dir})
include_directories(../DataStructure)

add_executable(RunTest ./DataStructureTest/TestSmallVector.cpp)

target_link_libraries(RunTest CppLab)
target_link_libraries(RunTest gtest gtest_main)