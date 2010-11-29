
if (NOT BLACKFIN)
  set(FFTW_INCLUDE_DIRS ${RAM_ROOT_DIR}/include)
  set(FFTW_LIBRARY_DIRS ${RAM_ROOT_DIR}/lib)
  find_package(FFTW REQUIRED)
  include_directories(${FFTW_INCLUDE_DIRS})
endif ()
