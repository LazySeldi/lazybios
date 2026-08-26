# Fuzz targets for lazybios.
#
# These are built as a separate, sanitizer- and coverage-instrumented copy of
# the library so that enabling them never changes the flags of the installed
# shared library.

if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "LAZYBIOS_BUILD_FUZZERS requires Clang (libFuzzer); got ${CMAKE_C_COMPILER_ID}")
endif()

set(LAZYBIOS_FUZZ_SANITIZERS "address,undefined" CACHE STRING
        "Sanitizers to build the fuzz targets with (comma separated, may be empty)")
option(LAZYBIOS_FUZZ_COVERAGE
        "Add source-coverage instrumentation for fuzz/coverage.sh" OFF)

set(LAZYBIOS_FUZZ_FLAGS -g -O1 -fno-omit-frame-pointer)
set(LAZYBIOS_FUZZ_LINK_FLAGS "")

if(LAZYBIOS_FUZZ_COVERAGE)
    list(APPEND LAZYBIOS_FUZZ_FLAGS -fprofile-instr-generate -fcoverage-mapping)
    list(APPEND LAZYBIOS_FUZZ_LINK_FLAGS -fprofile-instr-generate -fcoverage-mapping)
endif()

if(LAZYBIOS_FUZZ_SANITIZERS STREQUAL "")
    list(APPEND LAZYBIOS_FUZZ_FLAGS -fsanitize=fuzzer-no-link)
    list(APPEND LAZYBIOS_FUZZ_LINK_FLAGS -fsanitize=fuzzer)
else()
    list(APPEND LAZYBIOS_FUZZ_FLAGS
            -fsanitize=fuzzer-no-link,${LAZYBIOS_FUZZ_SANITIZERS}
            -fno-sanitize-recover=all)
    list(APPEND LAZYBIOS_FUZZ_LINK_FLAGS
            -fsanitize=fuzzer,${LAZYBIOS_FUZZ_SANITIZERS}
            -fno-sanitize-recover=all)
endif()

# Same sources as the shared library, referenced from the project root.
# LAZYBIOS_SOURCES mixes relative paths with absolute ones from file(GLOB),
# so only the relative entries may be rooted at the project directory.
set(LAZYBIOS_FUZZ_LIB_SOURCES "")
foreach(src ${LAZYBIOS_SOURCES})
    if(IS_ABSOLUTE "${src}")
        list(APPEND LAZYBIOS_FUZZ_LIB_SOURCES "${src}")
    else()
        list(APPEND LAZYBIOS_FUZZ_LIB_SOURCES "${PROJECT_SOURCE_DIR}/${src}")
    endif()
endforeach()

add_library(lazybios_fuzz_lib STATIC ${LAZYBIOS_FUZZ_LIB_SOURCES})

if(LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS)
    target_compile_definitions(lazybios_fuzz_lib PRIVATE
            ${LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS}
    )
endif()

target_include_directories(lazybios_fuzz_lib PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
target_include_directories(lazybios_fuzz_lib PRIVATE
        ${PROJECT_SOURCE_DIR}/src/internal
)

if(APPLE)
    target_link_libraries(lazybios_fuzz_lib PUBLIC
            ${LAZYBIOS_IOKIT_FRAMEWORK}
            ${LAZYBIOS_COREFOUNDATION_FRAMEWORK}
    )
endif()

# The library logs to stderr on every rejected input, which would bury the
# fuzzer's own output. The catch is that lb_dbg() compiles to nothing outside a
# debug build, so anything only reached from a lb_dbg() argument or condition
# (the entry point checksum verification, for one) is dead code the fuzzer can
# never enter. LAZYBIOS_FUZZ_LOGGING trades quiet output for that coverage.
option(LAZYBIOS_FUZZ_LOGGING "Build the fuzz targets against the debug-logging library" OFF)

if(LAZYBIOS_FUZZ_LOGGING)
    target_compile_definitions(lazybios_fuzz_lib PRIVATE LAZYBIOS_DEBUG)
else()
    target_compile_definitions(lazybios_fuzz_lib PRIVATE LAZYBIOS_QUIET)
endif()
target_compile_options(lazybios_fuzz_lib PRIVATE -Wall -Wextra -Wpedantic ${LAZYBIOS_FUZZ_FLAGS})
target_link_options(lazybios_fuzz_lib PUBLIC ${LAZYBIOS_FUZZ_LINK_FLAGS})
set_target_properties(lazybios_fuzz_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)

set(LAZYBIOS_FUZZ_TARGETS
        fuzz_dmi_table
        fuzz_entry_point
        fuzz_helpers
        fuzz_backend_buffers
        fuzz_json
)

if(UNIX)
    list(APPEND LAZYBIOS_FUZZ_TARGETS
            fuzz_single_file
            fuzz_two_files
    )
endif()

foreach(target ${LAZYBIOS_FUZZ_TARGETS})
    add_executable(${target} ${target}.c)
    target_link_libraries(${target} PRIVATE lazybios_fuzz_lib)
    if(LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS)
        target_compile_definitions(${target} PRIVATE
                ${LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS}
        )
    endif()
    target_include_directories(${target} PRIVATE
            ${PROJECT_SOURCE_DIR}/src/internal
    )
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic ${LAZYBIOS_FUZZ_FLAGS})
endforeach()

if(BUILD_TESTING AND UNIX)
    add_test(NAME lazybios_fuzz_regressions
            COMMAND ${PROJECT_SOURCE_DIR}/fuzz/run_regressions.sh ${PROJECT_BINARY_DIR})
endif()

message(STATUS "lazybios: fuzz targets enabled (sanitizers: '${LAZYBIOS_FUZZ_SANITIZERS}')")
