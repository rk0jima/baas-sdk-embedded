cmake_minimum_required(VERSION 3.1)
project(embeddedsdk)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQURED ON)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "-std=c++11 -DELPP_THREAD_SAFE -DELPP_STL_LOGGING -DNB_DEBUG -O0 -g3")
else()
    set(CMAKE_CXX_FLAGS "-std=c++11 -DELPP_THREAD_SAFE -DELPP_STL_LOGGING")
endif()

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

set(CMAKE_INSTALL_LIBDIR lib)
add_subdirectory(libs/json)
add_subdirectory(libs/curlpp)

include_directories(${PROJECT_SOURCE_DIR}/libs/easyloggingpp/src)
include_directories(${PROJECT_SOURCE_DIR}/libs/json/include)
include_directories(${PROJECT_SOURCE_DIR}/libs/curlpp/include)
include_directories(${PROJECT_SOURCE_DIR}/include/)
add_library(embeddedsdk SHARED ${LIB_SOURCE_FILES})

install(TARGETS embeddedsdk LIBRARY DESTINATION lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/necbaas/ DESTINATION "include/necbaas")

