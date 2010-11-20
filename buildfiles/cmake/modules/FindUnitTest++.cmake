# Locate UNITTEST
# This module defines
# UnitTest++_LIB_DIR
# UnitTest+_FOUND 
# UnitTest++_INCLUDE_DIR, where to find the headers
#

FIND_PATH(UnitTest++_INCLUDE_DIR UnitTest++/UnitTest++.h
  /opt/ram/local/include
  /usr/local/include
  /usr/include
  /opt/local/include # DarwinPorts
  /opt/include
  )

FIND_LIBRARY(UnitTest++_LIB_DIR
  NAMES UnitTest++
  PATHS
  /opt/ram/local/lib
  /usr/local/lib
  /usr/lib
  /opt/local/lib
  )

IF(UnitTest++_LIB_DIR AND UnitTest++_INCLUDE_DIR)
  MESSAGE(STATUS "Found UnitTest++")
  SET(UnitTest++_FOUND TRUE)
ELSE(UnitTest++_LIB_DIR AND UnitTest++_INCLUDE_DIR)
  SET(UnitTest++_FOUND FALSE)
  IF(UnitTest++_FIND_REQUIRED)
    MESSAGE(SEND_ERROR "UnitTest++ Not Found")  
  ENDIF(UnitTest++_FIND_REQUIRED)
ENDIF(UnitTest++_LIB_DIR AND UnitTest++_INCLUDE_DIR)
