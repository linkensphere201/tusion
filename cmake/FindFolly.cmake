find_path(Folly_INCLUDE_DIR NAMES folly)
find_library(Folly_LIBRARY NAMES libfolly.a libfollybenchmark.a)

if(Folly_INCLUDE_DIR AND Folly_LIBRARY)
    set(Folly_FOUND TRUE)
    message(STATUS "Found folly include: " ${Folly_INCLUDE_DIR})
    message(STATUS "Found folly library: " ${Folly_LIBRARY})
endif()

if(NOT Folly_FOUND)
    message(FATAL_ERROR "Folly doesn't exist")
endif()
