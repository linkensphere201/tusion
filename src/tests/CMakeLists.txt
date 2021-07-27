file(GLOB TEST_SOURCES test_*)
foreach(TST_SINGLE_SRC ${TEST_SOURCES})
    get_filename_component(TST_SINGLE_TARGET ${TST_SINGLE_SRC} NAME_WE)
    message("add test target: ${TST_SINGLE_TARGET}, source: ${TST_SINGLE_SRC}")
    tusion_add_executable(
        NAME
            ${TST_SINGLE_TARGET}
        SOURCES
            ${TST_SINGLE_SRC}
        OBJECTS
            $<TARGET_OBJECTS:tusion_util_obj>
            $<TARGET_OBJECTS:tusion_bench_obj>
            $<TARGET_OBJECTS:tusion_gen_obj>
        IS_TEST
    )
endforeach()
