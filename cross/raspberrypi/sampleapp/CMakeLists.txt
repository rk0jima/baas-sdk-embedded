cmake_minimum_required(VERSION 2.8.4)
project(sampleMain)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQURED ON)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "-std=c++11 -DNB_DEBUG -O0 -g3")
else()
    set(CMAKE_CXX_FLAGS "-std=c++11")
endif()

include_directories(${PROJECT_SOURCE_DIR}/embeddedsdk/include)
include_directories(${RASPBERRYPI_ROOT}/rootfs/usr/include)
link_directories(${PROJECT_SOURCE_DIR}/embeddedsdk/lib)
link_directories(${RASPBERRYPI_ROOT}/rootfs/lib/arm-linux-gnueabihf)
link_directories(${RASPBERRYPI_ROOT}/rootfs/usr/lib/arm-linux-gnueabihf)

add_executable (sampleMain main.cpp)
target_link_libraries(sampleMain embeddedsdk)
target_link_libraries(sampleMain jsoncpp)
target_link_libraries(sampleMain curlpp)

