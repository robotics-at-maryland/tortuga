
if ("$ENV{RAM_SVN_DIR}" STREQUAL "")
  message(SEND_ERROR "R@M Environment Not Setup. Bootstrap this terminal.")
elseif (NOT "$ENV{RAM_SVN_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  message(WARNING "Source directory differs from RAM_SVN_DIR")
endif ()
