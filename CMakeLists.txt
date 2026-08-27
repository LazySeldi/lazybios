cmake_minimum_required(VERSION 3.15)

project(lazybios
        VERSION 3.0.0
        DESCRIPTION "Lightweight SMBIOS/DMI parsing library"
        LANGUAGES C
        HOMEPAGE_URL "https://github.com/LazySeldi/lazybios"
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include(GNUInstallDirs)

file(GLOB OEM_SOURCES "src/structures/oem/*/*.c")

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
        ${OEM_SOURCES}
        src/json/cJSON.c
        src/json/lazybios_json.c
)

include(cmake-files/reactOS.cmake)
set(LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS "")

if(REACTOS OR CMAKE_SYSTEM_NAME STREQUAL "ReactOS")
    list(APPEND LAZYBIOS_SOURCES src/backends/reactos.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_REACTOS=1)
    set(SELECTED_BACKEND "src/backends/reactos.c")
elseif(WIN32 OR CYGWIN)
    list(APPEND LAZYBIOS_SOURCES src/backends/windows.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_WINDOWS=1)
    set(SELECTED_BACKEND "src/backends/windows.c")
elseif(APPLE)
    list(APPEND LAZYBIOS_SOURCES src/backends/macos.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_MACOS=1)
    set(SELECTED_BACKEND "src/backends/macos.c")
elseif(HAIKU)
    list(APPEND LAZYBIOS_SOURCES src/backends/haiku.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_HAIKU=1)
    set(SELECTED_BACKEND "src/backends/haiku.c")
elseif(BEOS)
    list(APPEND LAZYBIOS_SOURCES src/backends/beos.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_BEOS=1)
    set(SELECTED_BACKEND "src/backends/beos.c")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR ANDROID)
    list(APPEND LAZYBIOS_SOURCES src/backends/linux.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_LINUX=1)
    set(SELECTED_BACKEND "src/backends/linux.c")
elseif(CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
    list(APPEND LAZYBIOS_SOURCES src/backends/openbsd.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_OPENBSD=1)
    set(SELECTED_BACKEND "src/backends/openbsd.c")
elseif(CMAKE_SYSTEM_NAME MATCHES "(FreeBSD|MidnightBSD)")
    list(APPEND LAZYBIOS_SOURCES src/backends/freebsd.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_FREEBSD=1)
    set(SELECTED_BACKEND "src/backends/freebsd.c")
elseif(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    list(APPEND LAZYBIOS_SOURCES src/backends/netbsd.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_NETBSD=1)
    set(SELECTED_BACKEND "src/backends/netbsd.c")
elseif(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
    list(APPEND LAZYBIOS_SOURCES src/backends/sunos.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_SUNOS=1)
    set(SELECTED_BACKEND "src/backends/sunos.c")
elseif(CMAKE_SYSTEM_NAME STREQUAL "DragonFly")
    list(APPEND LAZYBIOS_SOURCES src/backends/dragonfly.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_DRAGONFLY=1)
    set(SELECTED_BACKEND "src/backends/dragonfly.c")
elseif(CMAKE_SYSTEM_NAME STREQUAL "QNX")
    list(APPEND LAZYBIOS_SOURCES src/backends/qnx.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_QNX=1)
    set(SELECTED_BACKEND "src/backends/qnx.c")
elseif(CMAKE_SYSTEM_NAME MATCHES "(Minix|MINIX)")
    list(APPEND LAZYBIOS_SOURCES src/backends/minix.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_MINIX=1)
    set(SELECTED_BACKEND "src/backends/minix.c")
else()
    message(WARNING "Unknown platform detected. Falling back to generic /dev/mem backend.")
    list(APPEND LAZYBIOS_SOURCES src/backends/generic.c)
    list(APPEND LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS OS_GENERIC=1)
    set(SELECTED_BACKEND "src/backends/generic.c")
endif()

add_library(lazybios SHARED ${LAZYBIOS_SOURCES})
target_compile_definitions(lazybios PRIVATE ${LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS})

if(WIN32)
    set_target_properties(lazybios PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(APPLE)
    find_library(IOKIT IOKit REQUIRED)
    find_library(COREFOUNDATION CoreFoundation REQUIRED)
    target_link_libraries(lazybios PRIVATE ${IOKIT} ${COREFOUNDATION})
endif()

# Options
option(LAZYBIOS_QUIET "Disable logging" OFF)
option(LAZYBIOS_DEBUG "Enable debug logging" OFF)

if(LAZYBIOS_QUIET)
    target_compile_definitions(lazybios PRIVATE LAZYBIOS_QUIET)
elseif(LAZYBIOS_DEBUG)
    target_compile_definitions(lazybios PRIVATE LAZYBIOS_DEBUG)
endif()

# EXTENTIONS
add_executable(lazybios_json_test test/test_json.c)
target_link_libraries(lazybios_json_test PRIVATE lazybios)

set_target_properties(lazybios PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        POSITION_INDEPENDENT_CODE ON
)

target_include_directories(lazybios PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_include_directories(lazybios PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)

if(MSVC)
    target_compile_options(lazybios PRIVATE /O2)
else()
    target_compile_options(lazybios PRIVATE -Wall -Wextra -Wpedantic -Werror -O2)
endif()

add_executable(lazybios_test test/test.c test/test_oem.c)
target_compile_definitions(lazybios_test PRIVATE TEST_OEM_NO_MAIN)

# OEM FIELDS
target_sources(lazybios PRIVATE ${OEM_SOURCES})
file(GLOB OEM_TEST_SOURCES "test/oem/*/*.c")
target_sources(lazybios_test PRIVATE ${OEM_TEST_SOURCES})
add_executable(lazybios_oem_test test/test_oem.c ${OEM_TEST_SOURCES})
target_link_libraries(lazybios_oem_test PRIVATE lazybios)

target_link_libraries(lazybios_test PRIVATE lazybios)

if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(lazybios_test PRIVATE /RTC1)
        target_compile_options(lazybios_json_test PRIVATE /RTC1)
    else()
        target_compile_options(lazybios_test PRIVATE /O2)
        target_compile_options(lazybios_json_test PRIVATE /O2)
    endif()
else()
    target_compile_options(lazybios_test PRIVATE -O2)
    target_compile_options(lazybios_json_test PRIVATE -O2)
endif()

include(CTest)
option(LAZYBIOS_TEST_HOST_BACKEND "Enable host-backend test" OFF)

if(BUILD_TESTING)
    add_executable(lazybios_semantic_test test/semantic_test.c)
    target_link_libraries(lazybios_semantic_test PRIVATE lazybios)
    target_compile_definitions(lazybios_semantic_test PRIVATE ${LAZYBIOS_PLATFORM_COMPILE_DEFINITIONS})
    target_include_directories(lazybios_semantic_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)
    
    if(NOT MSVC)
        target_compile_options(lazybios_semantic_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    
    add_test(NAME lazybios_semantics COMMAND lazybios_semantic_test)

    if(LAZYBIOS_TEST_HOST_BACKEND)
        add_executable(lazybios_host_backend_test test/host_backend_test.c)
        target_link_libraries(lazybios_host_backend_test PRIVATE lazybios)
        target_include_directories(lazybios_host_backend_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)
        if(NOT MSVC)
            target_compile_options(lazybios_host_backend_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
        endif()
        add_test(NAME lazybios_host_backend COMMAND lazybios_host_backend_test)
        set_tests_properties(lazybios_host_backend PROPERTIES SKIP_RETURN_CODE 77)
    endif()
endif()

if(LAZYBIOS_BUILD_FUZZERS)
    add_subdirectory(fuzz)
endif()

configure_file(cmake-files/lazybios.pc.in ${CMAKE_CURRENT_BINARY_DIR}/lazybios.pc @ONLY)

install(TARGETS lazybios EXPORT lazybiosTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY include/lazybios/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/lazybios FILES_MATCHING REGEX ".*\\.h$")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lazybios.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake-files/lazybiosConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/lazybiosConfig.cmake INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lazybios)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/lazybiosConfigVersion.cmake VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)

install(EXPORT lazybiosTargets FILE lazybiosTargets.cmake NAMESPACE lazybios:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lazybios)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lazybiosConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/lazybiosConfigVersion.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lazybios)

configure_file(cmake-files/cmake_uninstall.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake COMMENT "Uninstalling lazybios...")

message(STATUS "--------------------------------------------------------------------------------")
message(STATUS "lazybios ${PROJECT_VERSION} configured successfully!")
message(STATUS "")
message(STATUS "Installation Path: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Backend Selected:  ${SELECTED_BACKEND}")
message(STATUS "")
message(STATUS "Build targets:")
# Documentation. The version is injected here so the Doxyfile holds only one
# copy of the project's settings and none of its version.
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    add_custom_target(docs
            COMMAND ${CMAKE_COMMAND} -E echo "Generating documentation for ${PROJECT_NAME} ${PROJECT_VERSION}"
            COMMAND ${CMAKE_COMMAND} -DDOXYFILE=${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
                    -DVERSION=${PROJECT_VERSION} -DOUTDIR=${CMAKE_CURRENT_BINARY_DIR}
                    -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake-files/RunDoxygen.cmake
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Running Doxygen"
            VERBATIM)
endif()

message(STATUS "  make           - Build library, tests, and extensions")
message(STATUS "  make test      - Run automated tests")
message(STATUS "  make install   - Install library and headers")
message(STATUS "  make uninstall - Uninstall library and headers")
message(STATUS "")
message(STATUS "Usage:")
message(STATUS "  find_package(lazybios REQUIRED)")
message(STATUS "  target_link_libraries(your_target PRIVATE lazybios::lazybios)")
message(STATUS "--------------------------------------------------------------------------------")
