tusion_find_package(
    LIBNAME
        nebula_graph_client
    INCLUDE_DIR_NAMES
        nebula
    LIBRARY_FILE_NAMES
        libnebula_graph_client.so
)

tusion_print_config(nebula_graph_client_INCLUDES  "* depend")
tusion_print_config(nebula_graph_client_LIBRARIES "* depend")
