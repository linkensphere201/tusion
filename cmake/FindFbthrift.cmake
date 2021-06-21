tusion_find_package(
    LIBNAME
        fbthrift
    BINNAME
        thrift1
    INCLUDE_DIR_NAMES
        thrift
    LIBRARY_FILE_NAMES
        libthriftcpp2.a
)

tusion_print_config(fbthrift_BIN       "* depend")
tusion_print_config(fbthrift_INCLUDES  "* depend")
tusion_print_config(fbthrift_LIBRARIES "* depend")
