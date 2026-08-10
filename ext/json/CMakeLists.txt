find_package(PkgConfig REQUIRED)
pkg_check_modules(CJSON REQUIRED IMPORTED_TARGET libcjson)

add_library(lazybios_json SHARED src/lazybios_json.c)
target_link_libraries(lazybios_json PUBLIC lazybios PkgConfig::CJSON)
target_include_directories(lazybios_json PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS lazybios_json
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES include/lazybios_json.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/lazybios/ext/json)
