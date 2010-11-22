
set(RAM_CMAKE_DIR ${CMAKE_SOURCE_DIR}/buildfiles/cmake)
set(CMAKE_MODULE_PATH ${RAM_CMAKE_DIR}/modules)

set(RAM_ROOT_DIR "/opt/ram/local" CACHE STRING "Location of external dependencies")

include(${RAM_CMAKE_DIR}/helpers.cmake)
include(${RAM_CMAKE_DIR}/package-boost.cmake)
include(${RAM_CMAKE_DIR}/package-log4cpp.cmake)
include(${RAM_CMAKE_DIR}/package-python.cmake)
include(${RAM_CMAKE_DIR}/package-unittestpp.cmake)
include(${RAM_CMAKE_DIR}/package-wxwidgets.cmake)
include(${RAM_CMAKE_DIR}/package-opencv.cmake)
include(${RAM_CMAKE_DIR}/package-fftw.cmake)
include(${RAM_CMAKE_DIR}/package-dc1394.cmake)
include(${RAM_CMAKE_DIR}/package-fann.cmake)
include(${RAM_CMAKE_DIR}/package-segment.cmake)

include_directories(
  ${CMAKE_SOURCE_DIR}/packages
  )

set(LIBDIR ${CMAKE_BINARY_DIR}/lib)
set(TESTDIR ${CMAKE_BINARY_DIR}/tests)

if (UNIX)
  add_definitions(-Wall -Werror -DRAM_POSIX -DRAM_LINUX)
elseif (WIN32)
  add_definitions(-DRAM_WINDOWS)
endif ()
add_definitions(-DBOOST_PYTHON_NO_PY_SIGNATURES -fmessage-length=0)
