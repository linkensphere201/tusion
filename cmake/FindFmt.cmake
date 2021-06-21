tusion_find_package(
    LIBNAME
        fmt
    INCLUDE_DIR_NAMES
        fmt
    LIBRARY_FILE_NAMES
        libfmt.a
)
tusion_print_config(fmt_INCLUDES  "* depend")
tusion_print_config(fmt_LIBRARIES "* depend")
