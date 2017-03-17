set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/gtest)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/gmock)

link_directories(${CMAKE_BINARY_DIR}/libs)
link_directories(${CMAKE_BINARY_DIR}/libs/curlpp)
link_directories(${CMAKE_BINARY_DIR}/libs/json/src/lib_json)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/gtest)

link_libraries(curl)

set(TEST_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_service_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_utility_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_user_entity_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_session_token_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_rest_executor_pool_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_logger_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_http_request_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_http_request_factory_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_file_metadata_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_http_response_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_acl_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_query_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_http_handler_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_http_file_download_handler_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_http_file_upload_handler_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_json_object_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_json_array_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_rest_executor_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_user_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_api_gateway_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_file_bucket_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_object_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/nb_object_bucket_test.cc
    )

add_executable(unit_test ${TEST_FILES})

target_link_libraries(unit_test embeddedsdk gmock gmock_main gtest gtest_main pthread jsoncpp curlpp)

add_test(NAME ut COMMAND $<TARGET_FILE:unit_test>)
