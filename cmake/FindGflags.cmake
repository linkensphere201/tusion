tusion_find_package(
    LIBNAME
        gflags
    INCLUDE_DIR_NAMES
        gflags
    LIBRARY_FILE_NAMES
        libgflags.a
)

tusion_print_config(gflags_INCLUDES   "* depend")
tusion_print_config(gflags_LIBRARIES  "* depend")
