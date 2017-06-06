cmake_minimum_required(VERSION 3.1)
project(embeddedsdk)

option(IA32 "Build for IA-32 architectures" OFF)
option(ARM_TEST "Build for testing on the ARM architectures" OFF)
option(UNIT_TESTS "Compile unit tests" OFF)
option(FUNCTIONAL_TESTS "Compile functional tests" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQURED ON)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "-std=c++11 -DNB_DEBUG -O0 -g3")
else()
    set(CMAKE_CXX_FLAGS "-std=c++11")
endif()

if(IA32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
endif()

# バージョン番号
set(serial 6.2.0)
# 共有ライブラリのバージョン番号
set(soserial 6)

set(LIB_SOURCE_FILES
    src/nb_http_response.cc
    src/nb_json_array.cc
    src/nb_json_object.cc
    src/nb_service.cc
    src/nb_user.cc
    src/nb_api_gateway.cc
    src/nb_acl_base.cc
    src/nb_acl.cc
    src/nb_file_metadata.cc
    src/nb_file_bucket.cc
    src/nb_query.cc
    src/nb_object.cc
    src/nb_object_bucket.cc
    src/internal/nb_constants.cc
    src/internal/nb_http_request.cc
    src/internal/nb_http_request_factory.cc
    src/internal/nb_http_file_download_handler.cc
    src/internal/nb_http_file_upload_handler.cc
    src/internal/nb_http_handler.cc
    src/internal/nb_logger.cc
    src/internal/nb_rest_executor.cc
    src/internal/nb_rest_executor_pool.cc
    src/internal/nb_session_token.cc
    src/internal/nb_user_entity.cc
    src/internal/nb_utility.cc
    )

# easylogging++.hをSDK環境に取り込む
file(COPY libs/easyloggingpp/src/easylogging++.h DESTINATION ${PROJECT_SOURCE_DIR}/include/necbaas/internal)

set(CMAKE_INSTALL_LIBDIR lib)

# JSONCPPのテストを無効化
set(JSONCPP_WITH_TESTS OFF CACHE BOOL "Compile and (for jsoncpp_check) run JsonCpp test executables")
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "Automatically run unit-tests as a post build step")
add_subdirectory(libs/json)
add_subdirectory(libs/curlpp)

include_directories(${PROJECT_SOURCE_DIR}/libs/easyloggingpp/src)
include_directories(${PROJECT_SOURCE_DIR}/libs/json/include)
include_directories(${PROJECT_SOURCE_DIR}/libs/curlpp/include)
include_directories(${PROJECT_SOURCE_DIR}/include/)
add_library(embeddedsdk SHARED ${LIB_SOURCE_FILES})

set_target_properties(embeddedsdk PROPERTIES VERSION ${serial} SOVERSION ${soserial})


install(TARGETS embeddedsdk LIBRARY DESTINATION lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/necbaas/ DESTINATION "include/necbaas")

if(UNIT_TESTS)
    enable_testing()
    add_subdirectory(unit_tests)
endif()
if(FUNCTIONAL_TESTS)
    enable_testing()
    add_subdirectory(functional_tests)
endif()
