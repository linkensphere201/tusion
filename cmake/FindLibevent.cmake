tusion_find_package(
    LIBNAME
        libevent
    INCLUDE_DIR_NAMES
        event2
        event.h
    LIBRARY_FILE_NAMES
        libevent.a
        libevent_core.a
        libevent_extra.a
)
tusion_print_config(libevent_INCLUDES   "* depend")
tusion_print_config(libevent_LIBRARIES  "* depend")
