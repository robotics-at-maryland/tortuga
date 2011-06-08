
if (NOT BLACKFIN)
  set(OpenCV_DIR /usr)
  find_package(OpenCV REQUIRED)
  include_directories(${OpenCV_INCLUDE_DIR})
endif ()
