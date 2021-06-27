include_directories(AFTER tusion)

add_subdirectory(tusion)
add_subdirectory(tests)

tusion_add_executable(
    NAME
        t1
    SOURCES
        t_main.cc
    OBJECTS
        $<TARGET_OBJECTS:tusion_util_obj>
)

tusion_add_executable(
    NAME
        e2enbench
    SOURCES
        e2e_bench_nebula.cc
    OBJECTS
        $<TARGET_OBJECTS:tusion_util_obj>
        $<TARGET_OBJECTS:tusion_bench_obj>
)

