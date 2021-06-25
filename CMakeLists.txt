cmake_minimum_required(VERSION 3.5.0)
project("tusion" C CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(TusionMacro)
include(TusionConfig)

tusion_print_environment_variables()
add_definitions(-DTUSIONHOME=${CMAKE_SOURCE_DIR})

find_package(Fbthrift REQUIRED)
find_package(Folly REQUIRED)
find_package(GtestAndGmock REQUIRED)
find_package(Gflags REQUIRED)
find_package(Glog REQUIRED)
find_package(Spdlog REQUIRED)
find_package(Libevent REQUIRED)
find_package(DoubleConversion REQUIRED)
find_package(Fmt REQUIRED)
find_package(BoostContext REQUIRED)
find_package(NebulaClients REQUIRED)

include_directories(SYSTEM ${CMAKE_INCLUDE_PATH})
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src)
