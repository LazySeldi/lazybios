# Runs Doxygen with PROJECT_NUMBER supplied from the CMake project version, so
# the Doxyfile itself never carries a copy of the version.
file(READ "${DOXYFILE}" _cfg)
set(_cfg "${_cfg}\nPROJECT_NUMBER = ${VERSION}\n")
file(WRITE "${OUTDIR}/Doxyfile.generated" "${_cfg}")
execute_process(COMMAND doxygen "${OUTDIR}/Doxyfile.generated" RESULT_VARIABLE _rc)
if(NOT _rc EQUAL 0)
    message(FATAL_ERROR "doxygen failed (${_rc})")
endif()
