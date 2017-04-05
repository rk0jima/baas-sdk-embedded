set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/unit_tests/include)

link_directories(${CMAKE_BINARY_DIR}/libs)
link_directories(${CMAKE_BINARY_DIR}/libs/curlpp)
link_directories(${CMAKE_BINARY_DIR}/libs/json/src/lib_json)
if(IA32)
    link_directories(${PROJECT_SOURCE_DIR}/unit_tests/libs/gtest/32)
else()
    link_directories(${PROJECT_SOURCE_DIR}/unit_tests/libs/gtest/64)
endif()

link_libraries(curl)

set(TEST_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/ft_util.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_user_ft.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_object_ft.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_file_bucket_ft.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_api_gateway_ft.cc
    )

add_executable(funcutional_test ${TEST_FILES})

target_link_libraries(funcutional_test embeddedsdk gtest  pthread jsoncpp curlpp)

add_test(NAME ft COMMAND $<TARGET_FILE:funcutional_test>)
