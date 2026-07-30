cmake_minimum_required(VERSION 3.15)

# Project definition
project(lazybios
        VERSION 1.3.0
        DESCRIPTION "Lightweight SMBIOS/DMI parsing library"
        LANGUAGES C
        HOMEPAGE_URL "https://github.com/LazySeldi/lazybios"
)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include(GNUInstallDirs)

# Library sources
set(LAZYBIOS_SOURCES
        src/backend.c
        src/lazybios.c
        src/structures/type0.c
        src/structures/type1.c
        src/structures/type2.c
        src/structures/type3.c
        src/structures/type4.c
        src/structures/type5.c
        src/structures/type6.c
        src/structures/type7.c
        src/structures/type8.c
        src/structures/type9.c
        src/structures/type10.c
        src/structures/type11.c
        src/structures/type12.c
        src/structures/type13.c
        src/structures/type14.c
        src/structures/type15.c
        src/structures/type16.c
        src/structures/type17.c
        src/structures/type18.c
        src/structures/type19.c
        src/structures/type20.c
        src/structures/type21.c
        src/structures/type22.c
        src/structures/type23.c
        src/structures/type24.c
        src/structures/type25.c
        src/structures/type26.c
        src/structures/type27.c
        src/structures/type28.c
        src/structures/type29.c
        src/structures/type30.c
        src/structures/type31.c
        src/structures/type32.c
        src/structures/type33.c
        src/structures/type34.c
        src/structures/type35.c
        src/structures/type36.c
        src/structures/type37.c
        src/structures/type38.c
        src/structures/type39.c
        src/structures/type40.c
        src/structures/type41.c
        src/structures/type42.c
        src/structures/type43.c
        src/structures/type44.c
        src/structures/type45.c
        src/structures/type46.c
)

set(LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS "")

if(WIN32)
    list(APPEND LAZYBIOS_SOURCES src/backends/windows.c)
elseif(APPLE)
    list(APPEND LAZYBIOS_SOURCES src/backends/macos.c)
elseif(HAIKU)
    list(APPEND LAZYBIOS_SOURCES src/backends/haiku.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_HAIKU=1)
elseif(BEOS)
    list(APPEND LAZYBIOS_SOURCES src/backends/beos.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_BEOS=1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND LAZYBIOS_SOURCES src/backends/linux.c)
elseif(CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
    list(APPEND LAZYBIOS_SOURCES src/backends/openbsd.c)
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    list(APPEND LAZYBIOS_SOURCES src/backends/freebsd.c)
elseif(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    list(APPEND LAZYBIOS_SOURCES src/backends/netbsd.c)
elseif(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
    list(APPEND LAZYBIOS_SOURCES src/backends/sunos.c)
elseif(CMAKE_SYSTEM_NAME STREQUAL "DragonFly")
    list(APPEND LAZYBIOS_SOURCES src/backends/dragonfly.c)
else()
    message(WARNING "Unknown platform detected. Falling back to generic /dev/mem backend.")
    list(APPEND LAZYBIOS_SOURCES src/backends/generic.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_GENERIC=1)
endif()

add_library(lazybios SHARED ${LAZYBIOS_SOURCES})

if(WIN32)
    # MinGW exports undecorated C symbols by default, while MSVC does not.
    # Generate the export table so the same public API is available from
    # native MSVC-built DLLs without compiler-specific annotations in every
    # public declaration.
    set_target_properties(lazybios PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS)
    target_compile_definitions(lazybios PRIVATE
            ${LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS}
    )
endif()

if(APPLE)
    find_library(LAZYBIOS_IOKIT_FRAMEWORK IOKit)
    find_library(LAZYBIOS_COREFOUNDATION_FRAMEWORK CoreFoundation)

    if(NOT LAZYBIOS_IOKIT_FRAMEWORK OR NOT LAZYBIOS_COREFOUNDATION_FRAMEWORK)
        message(FATAL_ERROR "lazybios requires the IOKit and CoreFoundation frameworks on macOS")
    endif()

    target_link_libraries(lazybios PRIVATE
            ${LAZYBIOS_IOKIT_FRAMEWORK}
            ${LAZYBIOS_COREFOUNDATION_FRAMEWORK}
    )
endif()

# Logging Options
option(LAZYBIOS_QUIET "Disable all lazybios logging" OFF)
option(LAZYBIOS_DEBUG "Enable lazybios debug logging" OFF)

if(LAZYBIOS_QUIET)
    target_compile_definitions(lazybios PRIVATE LAZYBIOS_QUIET)
    message(STATUS "lazybios: Quiet mode enabled (no logging)")
endif()

if(LAZYBIOS_DEBUG)
    target_compile_definitions(lazybios PRIVATE LAZYBIOS_DEBUG)
    message(STATUS "lazybios: Debug mode enabled")
endif()

if(NOT LAZYBIOS_QUIET AND NOT LAZYBIOS_DEBUG)
    message(STATUS "lazybios: Normal logging enabled")
endif()

set_target_properties(lazybios PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        POSITION_INDEPENDENT_CODE ON
)

target_include_directories(lazybios PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_include_directories(lazybios PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/internal
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(lazybios PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(lazybios_test test/test.c)
target_link_libraries(lazybios_test PRIVATE lazybios)

include(CTest)
option(LAZYBIOS_TEST_HOST_BACKEND
        "Enable the native host-backend integration test (may require privileges)" OFF)

if(BUILD_TESTING)
    add_executable(lazybios_semantic_test test/semantic_test.c)
    target_link_libraries(lazybios_semantic_test PRIVATE lazybios)
    if(LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS)
        target_compile_definitions(lazybios_semantic_test PRIVATE
                ${LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS}
        )
    endif()
    target_include_directories(lazybios_semantic_test PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src/internal
    )
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(lazybios_semantic_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    add_test(NAME lazybios_semantics COMMAND lazybios_semantic_test)

    if(LAZYBIOS_TEST_HOST_BACKEND)
        add_executable(lazybios_host_backend_test test/host_backend_test.c)
        target_link_libraries(lazybios_host_backend_test PRIVATE lazybios)
        if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(lazybios_host_backend_test PRIVATE
                    -Wall -Wextra -Wpedantic -Werror)
        endif()
        add_test(NAME lazybios_host_backend COMMAND lazybios_host_backend_test)
        set_tests_properties(lazybios_host_backend PROPERTIES SKIP_RETURN_CODE 77)
    endif()
endif()

# Fuzzing (Clang + libFuzzer)
option(LAZYBIOS_BUILD_FUZZERS "Build the libFuzzer targets in fuzz/" OFF)

if(LAZYBIOS_BUILD_FUZZERS)
    add_subdirectory(fuzz)
endif()

configure_file(
        cmake-files/lazybios.pc.in
        ${CMAKE_CURRENT_BINARY_DIR}/lazybios.pc
        @ONLY
)

# Installation
install(TARGETS lazybios
        EXPORT lazybiosTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lazybios.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# CMake package config
include(CMakePackageConfigHelpers)

configure_package_config_file(
        cmake-files/lazybiosConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/lazybiosConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lazybios
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/lazybiosConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(EXPORT lazybiosTargets
        FILE lazybiosTargets.cmake
        NAMESPACE lazybios::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lazybios
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/lazybiosConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/lazybiosConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lazybios
)

# Uninstall target
configure_file(
        cmake-files/cmake_uninstall.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
        @ONLY
)

add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
        COMMENT "Uninstalling lazybios..."
)

message(STATUS "
lazybios ${PROJECT_VERSION} configured successfully!

Installation:
  make install      - Install to ${CMAKE_INSTALL_PREFIX}
  make uninstall    - Remove installed files

Build targets:
  make              - Build library and test executable
  make test         - Run tests

Usage in other projects:
  find_package(lazybios REQUIRED)
  target_link_libraries(your_target PRIVATE lazybios::lazybios)

Or with pkg-config:
  pkg-config --cflags --libs lazybios

The pkg-config file will be installed to: ${CMAKE_INSTALL_PREFIX}/pkgconfig/lazybios.pc
")
