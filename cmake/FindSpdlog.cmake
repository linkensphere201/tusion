tusion_find_package(
    LIBNAME
        spdlog
    INCLUDE_DIR_NAMES
        spdlog
    LIBRARY_FILE_NAMES
        libspdlog.a
)

tusion_print_config(spdlog_INCLUDES  "* depend")
tusion_print_config(spdlog_LIBRARIES "* depend")
