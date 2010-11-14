#
# Locate log4cpp include paths and libraries
# log4cpp can be found at http://log4cpp.sourceforge.net/
# Written by Manfred Ulz, manfred.ulz_at_tugraz.at

# This module defines
# LOG4CPP_INCLUDE_DIR, where to find ptlib.h, etc.
# LOG4CPP_LIBRARIES, the libraries to link against to use pwlib.
# LOG4CPP_FOUND, If false, don't try to use pwlib.

FIND_PATH(Log4CPP_INCLUDE_DIR log4cpp/Category.hh
  PATHS
  /opt/ram/local/include
  /usr/local/include
  /usr/include
  $ENV{ProgramFiles}/log4cpp/include
)

FIND_LIBRARY(Log4CPP_LIBRARIES log4cpp
  PATHS
  /opt/ram/local/lib
  /usr/local/lib
  /usr/lib
  $ENV{ProgramFiles}/log4cpp/lib
)

IF(Log4CPP_INCLUDE_DIR AND Log4CPP_LIBRARIES)
  SET(Log4CPP_FOUND TRUE)
  IF (NOT Log4CPP_FIND_QUIETLY)
    MESSAGE(STATUS "Found Log4CPP")
  ENDIF (NOT Log4CPP_FIND_QUIETLY)
ELSE(Log4CPP_INCLUDE_DIR AND Log4CPP_LIBRARIES)
  SET(Log4CPP_FOUND FALSE)
  IF (Log4CPP_FIND_REQUIRED)
    MESSAGE(SEND_ERROR "Log4CPP could not be found on the system")
  ENDIF (Log4CPP_FIND_REQUIRED)
ENDIF(Log4CPP_INCLUDE_DIR AND Log4CPP_LIBRARIES)

MARK_AS_ADVANCED(
  Log4CPP_INCLUDE_DIR
  Log4CPP_LIBRARIES
) 
