#project(GoogleTest)

add_subdirectory(Lib)
include_directories(${gtest_source_dir}/include ${gtest_source_dir})
include_directories(../DataStructure)
include_directories(../Utils)

add_executable(RunTest ./DataStructureTest/TestSmallVector.cpp UtilsTest/MemoryPoolTest.cpp UtilsTest/EventTest.cpp DataStructureTest/TestIntrusiveList.cpp)

target_link_libraries(RunTest CppLab)
target_link_libraries(RunTest gtest gtest_main)