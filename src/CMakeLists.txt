macro(tusion_add_executable)
    cmake_parse_arguments(
        tusion_exec                 # prefix
        ""                          # <options>
        "NAME"                      # <one_value_args>
        "SOURCES;OBJECTS;LIBRARIES" # <multi_value_args>
        ${ARGN}
    )
    add_executable(
        ${tusion_exec_NAME}
        ${tusion_exec_SOURCES}
        ${tusion_exec_OBJECTS}
    )

    target_link_libraries(
        ${tusion_exec_NAME}
        ${tusion_exec_LIBRARIES}
        ${Spdlog_LIBRARY}
    )
endmacro()

include(ConfigTusion)

add_subdirectory(tusion)
include_directories(AFTER tusion)

tusion_add_executable(
    NAME
        t1
    SOURCES
        t_main.cc
    OBJECTS
        $<TARGET_OBJECTS:tusion_obj>
)
