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

set(LAZYBIOS_FUZZ_FLAGS -g -O1 -fno-omit-frame-pointer)
set(LAZYBIOS_FUZZ_LINK_FLAGS "")

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
set(LAZYBIOS_FUZZ_LIB_SOURCES "")
foreach(src ${LAZYBIOS_SOURCES})
    list(APPEND LAZYBIOS_FUZZ_LIB_SOURCES ${PROJECT_SOURCE_DIR}/${src})
endforeach()

add_library(lazybios_fuzz_lib STATIC ${LAZYBIOS_FUZZ_LIB_SOURCES})

target_include_directories(lazybios_fuzz_lib PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
target_include_directories(lazybios_fuzz_lib PRIVATE
        ${PROJECT_SOURCE_DIR}/src/internal
)

# The library logs to stderr on every rejected input, which would bury the
# fuzzer's own output.
target_compile_definitions(lazybios_fuzz_lib PRIVATE LAZYBIOS_QUIET)
target_compile_options(lazybios_fuzz_lib PRIVATE -Wall -Wextra -Wpedantic ${LAZYBIOS_FUZZ_FLAGS})
target_link_options(lazybios_fuzz_lib PUBLIC ${LAZYBIOS_FUZZ_LINK_FLAGS})
set_target_properties(lazybios_fuzz_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)

set(LAZYBIOS_FUZZ_TARGETS
        fuzz_dmi_table
        fuzz_entry_point
        fuzz_single_file
        fuzz_decoders
)

foreach(target ${LAZYBIOS_FUZZ_TARGETS})
    add_executable(${target} ${target}.c)
    target_link_libraries(${target} PRIVATE lazybios_fuzz_lib)
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic ${LAZYBIOS_FUZZ_FLAGS})
endforeach()

message(STATUS "lazybios: fuzz targets enabled (sanitizers: '${LAZYBIOS_FUZZ_SANITIZERS}')")
