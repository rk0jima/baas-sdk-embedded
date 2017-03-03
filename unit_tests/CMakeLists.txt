set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR})

#set(TEST_ROOT .)
#include_directories(${TEST_ROOT}/include/)

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/gtest)

link_directories(${PROJECT_SOURCE_DIR}/libs)
link_directories(${PROJECT_BINARY_DIR}/libs)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/gtest/)

link_libraries(curl)

add_executable(unit_test ${CMAKE_CURRENT_SOURCE_DIR}/SampleTest.cpp)

target_link_libraries(unit_test embeddedsdk gtest gtest_main pthread jsoncpp curlpp)
