find_path(Spdlog_INCLUDE_DIR NAMES spdlog)
find_library(Spdlog_LIBRARY NAMES libspdlog.a)

if(Spdlog_INCLUDE_DIR AND Spdlog_LIBRARY)
    set(Spdlog_FOUND TRUE)
    message(STATUS "Found Spdlog include: " ${Spdlog_INCLUDE_DIR})
    message(STATUS "Found Spdlog library: " ${Spdlog_LIBRARY})
endif()

if(NOT Spdlog_FOUND)
    message(FATAL_ERROR "Spdlog doesn't exist")
endif()
