#----------------------------------------------------------------
# Generated CMake target import file for configuration "".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "armadillo" for configuration ""
set_property(TARGET armadillo APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(armadillo PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_NOCONFIG "-framework Accelerate;/Users/ddliu/anaconda/lib/libhdf5.dylib;/usr/lib/libpthread.dylib;/Users/ddliu/anaconda/lib/libz.dylib;/usr/lib/libm.dylib"
  IMPORTED_LOCATION_NOCONFIG "/usr/lib/libarmadillo.4.40.1.dylib"
  IMPORTED_SONAME_NOCONFIG "libarmadillo.4.dylib"
  )

list(APPEND _IMPORT_CHECK_TARGETS armadillo )
list(APPEND _IMPORT_CHECK_FILES_FOR_armadillo "/usr/lib/libarmadillo.4.40.1.dylib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
