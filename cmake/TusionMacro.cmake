macro(tusion_print_config name status_string)
    set (prefix ${status_string})
    if ("${status_string}" STREQUAL "")
        set(prefix "***")
    endif()
    message(STATUS "${prefix} ${name}:  ${${name}} ")
endmacro()

macro(tusion_print_environment_variables)
    tusion_print_config(CMAKE_CXX_STANDARD "")
    tusion_print_config(CMAKE_CXX_COMPILER "")
    tusion_print_config(CMAKE_CXX_COMPILER_ID "")
    tusion_print_config(CMAKE_BINARY_DIR "")
endmacro()

macro(tusion_add_executable)
    cmake_parse_arguments(
        tusion_exec                 # pref
        "IS_TEST"                   # opts
        "NAME"                      # one_value
        "SOURCES;OBJECTS;LIBRARIES" # multi_value
        ${ARGN}
    )
    message(STATUS "--- add executable: ${tusion_exec_NAME} ---")

    add_executable(
        ${tusion_exec_NAME}
        ${tusion_exec_SOURCES}
        ${tusion_exec_OBJECTS}
    )

    target_link_libraries(
        ${tusion_exec_NAME}
        ${tusion_exec_LIBRARIES}
        ${spdlog_LIBRARIES}
        ${folly_LIBRARIES}
        ${glog_LIBRARIES}
        ${gflags_LIBRARIES}
        ${libevent_LIBRARIES}
        ${double-conversion_LIBRARIES}
        ${fmt_LIBRARIES}
        ${boost_context_LIBRARIES}
        # ${nebula_graph_client_LIBRARIES}
        resolv
        dl
        -liberty
        -pthread
        -static-libstdc++
    )

    set_target_properties(
        ${tusion_exec_NAME}
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )

    if (${tusion_exec_IS_TEST})
        target_link_libraries(
            ${tusion_exec_NAME}
            ${Gtest_LIBRARIES}
        )
        set_target_properties(
            ${tusion_exec_NAME}
            PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
        )
    endif()
endmacro()

macro(tusion_find_package)
    cmake_parse_arguments(
      tusion_find_pack                             # pref
      ""                                           # opts
      "LIBNAME;BINNAME"                            # one_value
      "INCLUDE_DIR_NAMES;LIBRARY_FILE_NAMES"       # multi_value
      ${ARGN}
    )
    # message(STATUS "finding package [${tusion_find_pack_LIBNAME}] with include_dir \"${tusion_find_pack_INCLUDE_DIR_NAMES}\" and lib \"${tusion_find_pack_LIBRARY_FILE_NAMES}\"")
    if (NOT "${tusion_find_pack_BINNAME}" STREQUAL "")
        find_program(${tusion_find_pack_LIBNAME}_BIN NAMES ${tusion_find_pack_BINNAME})
        if (NOT ${tusion_find_pack_LIBNAME}_BIN)
            message(FATAL_ERROR "BINARY ${tusion_find_pack_BINNAME} not found!")
        endif()
        message(STATUS "Found ${tusion_find_pack_LIBNAME} bin: " "${${tusion_find_pack_LIBNAME}_BIN}")
    endif()

    set(${tusion_find_pack_LIBNAME}_INCLUDES "")
    set(${tusion_find_pack_LIBNAME}_LIBRARIES "")

    foreach(include_dir ${tusion_find_pack_INCLUDE_DIR_NAMES})
        set(incvar "incdir_${include_dir}")
        find_path(${incvar}
            NAMES ${include_dir}
            HINTS ${CMAKE_INCLUDE_PATH})
        # message(STATUS "append include path: ${${incvar}} for ${include_dir}")
        list(APPEND ${tusion_find_pack_LIBNAME}_INCLUDES ${${incvar}})
    endforeach()
    foreach(library_file ${tusion_find_pack_LIBRARY_FILE_NAMES})
        set(libvar "libfile_${library_file}")
        find_library(${libvar} NAMES ${library_file})
        # message(STATUS "append library file: ${${libvar}}")
        list(APPEND ${tusion_find_pack_LIBNAME}_LIBRARIES ${${libvar}})
    endforeach()

    list(REMOVE_DUPLICATES ${tusion_find_pack_LIBNAME}_INCLUDES)
    list(REMOVE_DUPLICATES ${tusion_find_pack_LIBNAME}_LIBRARIES)

    list(LENGTH ${tusion_find_pack_LIBNAME}_INCLUDES  SZ_INCS)
    list(LENGTH ${tusion_find_pack_LIBNAME}_LIBRARIES SZ_LIBS)
    if(SZ_INCS GREATER 0 AND SZ_LIBS GREATER 0)
        set(${tusion_find_pack_LIBNAME}_FOUND TRUE)
        message(STATUS "Found ${tusion_find_pack_LIBNAME}: ")
        message(STATUS " - includes : " "${${tusion_find_pack_LIBNAME}_INCLUDES}")
        message(STATUS " - libraries: " "${${tusion_find_pack_LIBNAME}_LIBRARIES}")
    endif()
    if(NOT ${tusion_find_pack_LIBNAME}_FOUND)
        message(FATAL_ERROR "library ${tusion_find_pack_LIBNAME} not found!")
    endif()
endmacro()
