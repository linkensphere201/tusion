include_directories(AFTER tusion)

add_subdirectory(tusion)
add_subdirectory(tests)

tusion_add_executable(
    NAME
        t1
    SOURCES
        t_main.cc
    OBJECTS
        $<TARGET_OBJECTS:tusion_obj>
)
