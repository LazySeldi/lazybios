cmake_minimum_required(VERSION 4.0)
project(lazybios C)

set(CMAKE_C_STANDARD 23)

add_library(lazybios SHARED lazybios.c)

add_executable(lazybios_test test.c)
target_link_libraries(lazybios_test lazybios)

set_target_properties(lazybios PROPERTIES 
    OUTPUT_NAME "lazybios"
    PREFIX "lib"
    SOVERSION 1
)

set_target_properties(lazybios_test PROPERTIES OUTPUT_NAME "lazybios_test")
