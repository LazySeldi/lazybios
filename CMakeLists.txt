cmake_minimum_required(VERSION 3.15)

# Project definition
project(lazybios
        VERSION 3.2.0
        DESCRIPTION "Lightweight SMBIOS/DMI parsing library"
        LANGUAGES C
        HOMEPAGE_URL "https://github.com/LazySeldi/lazybios"
)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Include GNU standard directories
include(GNUInstallDirs)

# Library sources
set(LAZYBIOS_SOURCES
        src/lazybios.c
        src/structures/type0.c
        src/structures/type1.c
        src/structures/type2.c
        src/structures/type3.c
        #        src/structures/type4.c
)

add_library(lazybios SHARED ${LAZYBIOS_SOURCES})

# Logging Options
option(LAZYBIOS_QUIET "Disable all lazybios logging" OFF)
option(LAZYBIOS_DEBUG "Enable lazybios debug logging" OFF)

# Set compile definitions based on options
if(LAZYBIOS_QUIET)
    target_compile_definitions(lazybios PRIVATE LAZYBIOS_QUIET)
    message(STATUS "lazybios: Quiet mode enabled (no logging)")
endif()

if(LAZYBIOS_DEBUG)
    target_compile_definitions(lazybios PRIVATE LAZYBIOS_DEBUG)
    message(STATUS "lazybios: Debug mode enabled")
endif()

# Show status
if(NOT LAZYBIOS_QUIET AND NOT LAZYBIOS_DEBUG)
    message(STATUS "lazybios: Normal logging enabled")
endif()

set_target_properties(lazybios PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        POSITION_INDEPENDENT_CODE ON
)

# Include directories
target_include_directories(lazybios PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Compiler options
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(lazybios PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Create test executable
add_executable(lazybios_test test/test.c)
target_link_libraries(lazybios_test PRIVATE lazybios)

# Generate pkg-config file
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

# Summary with pkg-config path info
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
