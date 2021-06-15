find_path(Fbthrift_INCLUDE_DIR NAMES thrift)
find_library(Fbthrift_LIBRARY NAMES libthriftcpp2.a)
find_program(Fbthrift_BIN NAMES thrift1)

if(Fbthrift_INCLUDE_DIR AND Fbthrift_LIBRARY AND Fbthrift_BIN)
    set(Fbthrift_FOUND TRUE)
    message(STATUS "Found fbthrift include: " ${Fbthrift_INCLUDE_DIR})
    message(STATUS "Found fbthrift library: " ${Fbthrift_LIBRARY})
    message(STATUS "Found fbthrift bin: " ${Fbthrift_BIN})
endif()

if(NOT Fbthrift_FOUND)
    message(FATAL_ERROR "Fbthrift doesn't exist")
endif()
