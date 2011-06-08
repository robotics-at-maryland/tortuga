
if (NOT BLACKFIN)
  set(CMAKE_FIND_ROOT_PATH ${RAM_ROOT_DIR})
  find_package(FFMPEG REQUIRED)
  include_directories(${FFMPEG_INCLUDE_DIR})
endif ()
