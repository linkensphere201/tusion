tusion_find_package(
    LIBNAME
        glog
    INCLUDE_DIR_NAMES
        glog
    LIBRARY_FILE_NAMES
        libglog.a
)
tusion_print_config(glog_INCLUDES  "* depend")
tusion_print_config(glog_LIBRARIES "* depend")
