cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

project(image_analyzer)

# Appends the cmake/modules path inside the MAKE_MODULE_PATH 
# variable which stores the directories of additional CMake 
# modules (eg MacroOutOfSourceBuild.cmake):

set(CMAKE_MODULE_PATH ${image_analyzer_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

# The macro below forces the build directory to be different from source directory:
include(MacroOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build.")

set(ROOTSYS /opt/ram/local/lib/root/)
include(FindROOT)

# Check the compiler and set the compile and link flags
set(CMAKE_BUILD_TYPE Debug)

set(OPEN_CV_INCLUDE_DIR /opt/ram/local/include/opencv)
set(OPEN_CV_LINK_DIR /opt/ram/local/lib)
set(LD_LIBRARY_PATH /opt/ram/local/lib ${LD_LIBRARY_PATH})

set(INCLUDE_DIRECTORIES ${ROOT_INCLUDE_DIR} ${OPEN_CV_INCLUDE_DIR})
include_directories(${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES ${ROOT_LIBRARY_DIR} ${OPEN_CV_LINK_DIR})
link_directories(${LINK_DIRECTORIES})

set(OPEN_CV_LIBS -L/opt/ram/local/lib -lcxcore -lcv -lcvaux -lml -lhighgui)

add_subdirectory(src)