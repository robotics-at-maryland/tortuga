project(robotics-at-maryland)
cmake_minimum_required(VERSION 2.6)

include(${CMAKE_SOURCE_DIR}/buildfiles/cmake/common.cmake)
set(FEATURE_HEADER ${CMAKE_SOURCE_DIR}/packages/core/include/Feature.h
  CACHE STRING "Feature header file location")
include(${RAM_CMAKE_DIR}/features.cmake)

# Setup a hook to rerun features.cmake if Features.h is modified
add_custom_command(
  OUTPUT _feature_list.cache
  COMMAND ${CMAKE_COMMAND} . -DFORCE_FEATURE_LIST=YES -P ${CMAKE_SOURCE_DIR}/buildfiles/cmake/features.cmake
  DEPENDS packages/core/include/Feature.h
  )
add_custom_target(feature_list ALL DEPENDS _feature_list.cache)

subdirs(packages/pattern)
subdirs(packages/drivers)
subdirs(packages/core)
subdirs(packages/math)
subdirs(packages/vehicle)
subdirs(packages/estimation)
subdirs(packages/control)
subdirs(packages/vision)
subdirs(packages/network)
subdirs(packages/logging)
subdirs(packages/sonar)

######             WRAPPERS             #####
if (RAM_WITH_WRAPPERS)
  include(${RAM_CMAKE_DIR}/wrappers.cmake)
  subdirs(wrappers/core)
  subdirs(wrappers/math)
  subdirs(wrappers/vehicle)
  subdirs(wrappers/estimation)
  subdirs(wrappers/control)
  subdirs(wrappers/vision)
  subdirs(wrappers/network)
  subdirs(wrappers/logging)
endif ()

subdirs(tools/vision_tool)
subdirs(tools/vision_viewer)
subdirs(tools/plot)
