#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "yaml-cpp" for configuration "RelWithDebInfo"
set_property(TARGET yaml-cpp APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(yaml-cpp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/yaml-cpp.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS yaml-cpp )
list(APPEND _IMPORT_CHECK_FILES_FOR_yaml-cpp "${_IMPORT_PREFIX}/lib/yaml-cpp.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
