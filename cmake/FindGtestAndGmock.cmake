tusion_find_package(
    LIBNAME
        Gtest
    INCLUDE_DIR_NAMES
        gtest
    LIBRARY_FILE_NAMES
        libgtest.a
        libgtest_main.a
)

tusion_find_package(
    LIBNAME
        GMock
    INCLUDE_DIR_NAMES
        gmock
    LIBRARY_FILE_NAMES
        libgmock.a
        libgmock_main.a
)

tusion_print_config(Gtest_INCLUDES   "* depend")
tusion_print_config(Gtest_LIBRARIES  "* depend" )
tusion_print_config(GMock_INCLUDES   "* depend")
tusion_print_config(GMock_LIBRARIES  "* depend")
