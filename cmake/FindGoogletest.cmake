find_path(Googletest_INCLUDE_DIR NAMES gmock gtest)
find_library(Googletest_LIBRARY NAMES libgmock.a libgmock_main.a libgtest.a libgtest_main.a)

if(Googletest_INCLUDE_DIR AND Googletest_LIBRARY)
    set(Googletest_FOUND TRUE)
    message(STATUS "Found googletest include: " ${Googletest_INCLUDE_DIR})
    message(STATUS "Found googletest library: " "${Googletest_LIBRARY}")
endif()

if(NOT Googletest_FOUND)
    message(FATAL_ERROR "Googletest doesn't exist")
endif()

