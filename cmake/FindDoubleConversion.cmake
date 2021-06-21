tusion_find_package(
    LIBNAME
        double-conversion
    INCLUDE_DIR_NAMES
        double-conversion
    LIBRARY_FILE_NAMES
        libdouble-conversion.a
)

tusion_print_config(double-conversion_INCLUDES  "* depend")
tusion_print_config(double-conversion_LIBRARIES "* depend")
