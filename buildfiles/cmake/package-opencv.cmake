
if (NOT BLACKFIN)
  set(OpenCV_DIR ${RAM_ROOT_DIR})
  find_package(OpenCV REQUIRED)
  include_directories(${OpenCV_INCLUDE_DIR})
endif ()
