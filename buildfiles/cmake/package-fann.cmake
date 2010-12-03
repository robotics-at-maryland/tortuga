
if (NOT BLACKFIN)
  find_package(FANN REQUIRED)
  include_directories(${FANN_INCLUDE_DIR})
endif ()
