#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "asmjit::asmtk" for configuration "Release"
set_property(TARGET asmjit::asmtk APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(asmjit::asmtk PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/asmtk.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS asmjit::asmtk )
list(APPEND _IMPORT_CHECK_FILES_FOR_asmjit::asmtk "${_IMPORT_PREFIX}/lib/asmtk.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
