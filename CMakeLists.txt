cmake_minimum_required(VERSION 3.15)
project(using_boost)

set(CMAKE_CXX_STANDARD 14)

#message(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/library/include
        ${CMAKE_CURRENT_SOURCE_DIR}/using_boost/inc)

file(GLOB SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/using_boost/*.cpp")
message(${SRC_LIST})
link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/library/static/boost)

add_executable(using_boost ${SRC_LIST})