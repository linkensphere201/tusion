cmake_minimum_required(VERSION 3.5.0)
project("tusion" C CXX)

message(STATUS "source dir: "  ${CMAKE_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

add_definitions(-DTUSIONHOME=${CMAKE_SOURCE_DIR})

find_package(Fbthrift REQUIRED)
find_package(Folly REQUIRED)
find_package(Gflags REQUIRED)
find_package(Glog REQUIRED)
find_package(Googletest REQUIRED)
find_package(Spdlog REQUIRED)


include_directories(SYSTEM ${CMAKE_INCLUDE_PATH})
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/include)
add_subdirectory(src)

