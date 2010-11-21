project(robotics-at-maryland)
cmake_minimum_required(VERSION 2.6)

include(${CMAKE_SOURCE_DIR}/buildfiles/cmake/common.cmake)
include(${RAM_CMAKE_DIR}/features.cmake)

subdirs(packages/pattern)
subdirs(packages/drivers)
subdirs(packages/core)
subdirs(packages/math)
subdirs(packages/vehicle)
subdirs(packages/control)
# subdirs(packages/network)

######             WRAPPERS             #####
include(${RAM_CMAKE_DIR}/wrappers.cmake)
subdirs(wrappers/core)
subdirs(wrappers/math)
subdirs(wrappers/vehicle)
subdirs(wrappers/control)
