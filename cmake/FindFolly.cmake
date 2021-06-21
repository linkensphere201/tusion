tusion_find_package(
    LIBNAME
        folly
    INCLUDE_DIR_NAMES
        folly
    LIBRARY_FILE_NAMES
        libfolly.a
        libfollybenchmark.a
)
tusion_print_config(folly_INCLUDES  "* depend")
tusion_print_config(folly_LIBRARIES "* depend")
