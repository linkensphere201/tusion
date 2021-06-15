# force find_path to find from ${CMAKE_INCLUDE_PATH}
find_path(
    Libevent_INCLUDE_DIR
    NAMES
        event2 event.h
    HINTS
        ${CMAKE_INCLUDE_PATH}
)
find_library(
    Libevent_LIBRARY
    NAMES
        libevent.a libevent_core.a libevent_extra.a
)

if(Libevent_INCLUDE_DIR AND Libevent_LIBRARY)
    set(Libevent_FOUND TRUE)
    message(STATUS "Found Libevent include: " ${Libevent_INCLUDE_DIR})
    message(STATUS "Found Libevent library: " ${Libevent_LIBRARY})
endif()

if(NOT Libevent_FOUND)
    message(FATAL_ERROR "libevent doesn't exist")
endif()
