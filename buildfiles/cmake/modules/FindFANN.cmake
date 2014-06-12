#
# Try to find the FANN library and includes files.
#
# FANN_INCLUDE_DIR, where to find fann.h
# FANN_LIBRARIES, the libraries to link against to use FANN
# FANN_FOUND, if false, fann was not found
#

find_path( FANN_INCLUDE_DIR fann.h
  ${RAM_ROOT_DIR}/include
  /usr/include
  /usr/local/include
)

find_library( FANN_LIBRARY fann
  ${RAM_ROOT_DIR}/lib
  /usr/lib
  /usr/local/lib
)

find_library( FLOATFANN_LIBRARY floatfann
  ${RAM_ROOT_DIR}/lib
  /usr/lib
  /usr/local/lib
  )

set( FANN_FOUND "NO" )
if( FANN_INCLUDE_DIR AND FANN_LIBRARY AND FLOATFANN_LIBRARY )
  message(STATUS "Found FANN: ${FANN_LIBRARY}")
  message(STATUS "Found FLOATFANN: ${FLOATFANN_LIBRARY}")
  set( FANN_LIBRARIES ${FANN_LIBRARY} ${FLOATFANN_LIBRARY} )
  set( FANN_FOUND "YES" )
endif( FANN_INCLUDE_DIR AND FANN_LIBRARY AND FLOATFANN_LIBRARY )

if(FANN_FIND_REQUIRED AND NOT FANN_FOUND)
  message(SEND_ERROR "Unable to find the requested FANN library")
endif (FANN_FIND_REQUIRED AND NOT FANN_FOUND)

mark_as_advanced(
  FANN_INCLUDE_DIR
  FANN_LIBRARIES
  )
