tusion_find_package(
    LIBNAME
        boost_context
    INCLUDE_DIR_NAMES
        boost
    LIBRARY_FILE_NAMES
        libboost_context.a
)

tusion_print_config(boost_context_INCLUDES  "* depend")
tusion_print_config(boost_context_LIBRARIES "* depend")
