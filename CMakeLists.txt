cmake_minimum_required(VERSION 3.10)
project(lazybios C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Install dirs
include(GNUInstallDirs)

# Set installation prefix
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Installation prefix" FORCE)
endif()

# Versioning
set(LAZYBIOS_VERSION_MAJOR 1)
set(LAZYBIOS_VERSION_MINOR 4)
set(LAZYBIOS_VERSION_PATCH 0)
set(LAZYBIOS_VERSION "${LAZYBIOS_VERSION_MAJOR}.${LAZYBIOS_VERSION_MINOR}.${LAZYBIOS_VERSION_PATCH}")

# Targets

# Shared library
add_library(lazybios SHARED lazybios.c)

# Test executable (NOT installable)
add_executable(lazybios_test test.c)
target_link_libraries(lazybios_test lazybios)

# Library properties
set_target_properties(lazybios PROPERTIES
    OUTPUT_NAME "lazybios"
    SOVERSION ${LAZYBIOS_VERSION_MAJOR}
    VERSION   ${LAZYBIOS_VERSION}
)

set_target_properties(lazybios_test PROPERTIES
    OUTPUT_NAME "lazybios_test"
)

# Install rules

install(TARGETS lazybios
    EXPORT lazybiosTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES lazybios.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# pkg-config
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/lazybios.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/lazybios.pc"
    @ONLY
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/lazybios.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# CMake package config

include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/lazybiosConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/lazybiosConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lazybios"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/lazybiosConfigVersion.cmake"
    VERSION ${LAZYBIOS_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/lazybiosConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/lazybiosConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lazybios"
)

# Export targets

install(EXPORT lazybiosTargets
    FILE lazybiosTargets.cmake
    NAMESPACE lazybios::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lazybios"
)

# Uninstall target

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    @ONLY
)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

# Status messages

message(STATUS "lazybios ${LAZYBIOS_VERSION} configured")
message(STATUS "Installation prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Library dir: ${CMAKE_INSTALL_LIBDIR}")
message(STATUS "Binary dir: ${CMAKE_INSTALL_BINDIR}")
message(STATUS "Include dir: ${CMAKE_INSTALL_INCLUDEDIR}")
message(STATUS "")
message(STATUS "Build targets:")
message(STATUS "  make              - Build library and test")
message(STATUS "  make install      - Install to ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  make uninstall    - Remove installed files")
message(STATUS "")
message(STATUS "Test the library:")
message(STATUS "  ./lazybios_test   - Run test program (development only)")
message(STATUS "")
message(STATUS "After installation:")
message(STATUS "  pkg-config --cflags lazybios  - Get compiler flags")
message(STATUS "  pkg-config --libs lazybios    - Get linker flags")

