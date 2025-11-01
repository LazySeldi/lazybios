cmake_minimum_required(VERSION 3.10)
project(lazybios C)

set(CMAKE_C_STANDARD 23)

# Set installation prefixes
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Installation prefix" FORCE)
endif()

# Library versioning
set(LAZYBIOS_VERSION_MAJOR 1)
set(LAZYBIOS_VERSION_MINOR 0)
set(LAZYBIOS_VERSION_PATCH 0)
set(LAZYBIOS_VERSION "${LAZYBIOS_VERSION_MAJOR}.${LAZYBIOS_VERSION_MINOR}.${LAZYBIOS_VERSION_PATCH}")

# Create the shared library
add_library(lazybios SHARED lazybios.c)

# Create the test executable (NOT installed)
add_executable(lazybios_test test.c)
target_link_libraries(lazybios_test lazybios)

# Set library properties
set_target_properties(lazybios PROPERTIES
        OUTPUT_NAME "lazybios"
        PREFIX "lib"
        SOVERSION ${LAZYBIOS_VERSION_MAJOR}
        VERSION ${LAZYBIOS_VERSION}
)

set_target_properties(lazybios_test PROPERTIES OUTPUT_NAME "lazybios_test")

# ===== INSTALLATION TARGETS =====

# Install the library to standard locations
install(TARGETS lazybios
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)

# Install the header file
install(FILES lazybios.h
        DESTINATION include
)

# Create pkg-config file for easier linking
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/lazybios.pc.in"
        "${CMAKE_CURRENT_BINARY_DIR}/lazybios.pc"
        @ONLY
)

# Install pkg-config file to standard location
if(CMAKE_INSTALL_PREFIX STREQUAL "/usr")
    set(PC_DIR "lib/pkgconfig")
else()
    set(PC_DIR "share/pkgconfig")
endif()

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/lazybios.pc"
        DESTINATION ${PC_DIR}
)

# ===== UNINSTALL TARGET =====

# Create uninstall target
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        @ONLY
)

add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

# ===== PACKAGE CONFIGURATION =====

# Generate and install package configuration for find_package() support
include(CMakePackageConfigHelpers)

configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/lazybiosConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/lazybiosConfig.cmake"
        INSTALL_DESTINATION "lib/cmake/lazybios"
)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/lazybiosConfigVersion.cmake"
        VERSION ${LAZYBIOS_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/lazybiosConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/lazybiosConfigVersion.cmake"
        DESTINATION "lib/cmake/lazybios"
)

# ===== EXPORT TARGETS =====

install(TARGETS lazybios
        EXPORT lazybiosTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)

install(EXPORT lazybiosTargets
        FILE lazybiosTargets.cmake
        NAMESPACE lazybios::
        DESTINATION "lib/cmake/lazybios"
)

# ===== USAGE INFORMATION =====

message(STATUS "lazybios ${LAZYBIOS_VERSION} configured")
message(STATUS "Installation prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "pkg-config location: ${PC_DIR}")
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