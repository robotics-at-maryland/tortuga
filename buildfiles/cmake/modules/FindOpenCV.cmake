###########################################################
#                  Find OpenCV Library
# See http://sourceforge.net/projects/opencvlibrary/
#----------------------------------------------------------
#
## 1: Setup:
# The following variables are optionally searched for defaults
#  OpenCV_DIR:            Base directory of OpenCv tree to use.
#
## 2: Variable
# The following are set after configuration is done: 
#  
#  OpenCV_FOUND
#  OpenCV_LIBS
#  OpenCV_INCLUDE_DIR
#  OpenCV_VERSION (OpenCV_VERSION_MAJOR, OpenCV_VERSION_MINOR, OpenCV_VERSION_PATCH)
#
#
# Deprecated variable are used to maintain backward compatibility with
# the script of Jan Woetzel (2006/09): www.mip.informatik.uni-kiel.de/~jw
#  OpenCV_INCLUDE_DIRS
#  OpenCV_LIBRARIES
#  OpenCV_LINK_DIRECTORIES
# 
## 3: Version
#
# 2010/04/07 Benoit Rat, Correct a bug when OpenCVConfig.cmake is not found.
# 2010/03/24 Benoit Rat, Add compatibility for when OpenCVConfig.cmake is not found.
# 2010/03/22 Benoit Rat, Creation of the script.
#
#
# tested with:
# - OpenCV 2.1:  MinGW, MSVC2008
# - OpenCV 2.0:  MinGW, MSVC2008, GCC4
#
#
## 4: Licence:
#
# LGPL 2.1 : GNU Lesser General Public License Usage
# Alternatively, this file may be used under the terms of the GNU Lesser

# General Public License version 2.1 as published by the Free Software
# Foundation and appearing in the file LICENSE.LGPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU Lesser General Public License version 2.1 requirements
# will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
# 
#----------------------------------------------------------


<<<<<<< Updated upstream
FIND_PATH(OPENCV_DIR OpenCVConfig.cmake /home/eliot/opencv-2.4.5/release NO_DEFAULT_PATH)
=======
FIND_PATH(OPENCV_DIR OpenCVConfig.cmake /home/stephenc/opencv/release)
>>>>>>> Stashed changes
message("OpenCV Firectory!!! : " ${OPENCV_DIR})

##====================================================
## Find OpenCV libraries
##----------------------------------------------------
if(EXISTS "${OPENCV_DIR}")

        #When its possible to use the Config script use it.
        if(EXISTS "${OPENCV_DIR}/OpenCVConfig.cmake")

                ## Include the standard CMake script
                include("${OPENCV_DIR}/OpenCVConfig.cmake")

                ## Search for a specific version
                set(CVLIB_SUFFIX "${OPENCV_VERSION_MAJOR}${OPENCV_VERSION_MINOR}${OPENCV_VERSION_PATCH}")

        #Otherwise it try to guess it.
        else(EXISTS "${OPENCV_DIR}/OpenCVConfig.cmake")
                set(OPENCV_LIB_COMPONENTS cxcore cv ml highgui cvaux)
                find_path(OPENCV_INCLUDE_DIR "cv.h" PATHS "${OPENCV_DIR}" PATH_SUFFIXES "include" "include/opencv" DOC "")
                if(EXISTS  ${OPENCV_INCLUDE_DIR})
                    include_directories(${OPENCV_INCLUDE_DIR})
                endif(EXISTS  ${OPENCV_INCLUDE_DIR})

                #Find OpenCV version by looking at cvver.h
                file(STRINGS ${OPENCV_INCLUDE_DIR}/cvver.h OPENCV_VERSIONS_TMP REGEX "^#define CV_[A-Z]+_VERSION[ \t]+[0-9]+$")
                string(REGEX REPLACE ".*#define CV_MAJOR_VERSION[ \t]+([0-9]+).*" "\\1" OPENCV_VERSION_MAJOR ${OPENCV_VERSIONS_TMP})
                string(REGEX REPLACE ".*#define CV_MINOR_VERSION[ \t]+([0-9]+).*" "\\1" OPENCV_VERSION_MINOR ${OPENCV_VERSIONS_TMP})
                string(REGEX REPLACE ".*#define CV_SUBMINOR_VERSION[ \t]+([0-9]+).*" "\\1" OPENCV_VERSION_PATCH ${OPENCV_VERSIONS_TMP})
                set(OPENCV_VERSION ${OPENCV_VERSION_MAJOR}.${OPENCV_VERSION_MINOR}.${OPENCV_VERSION_PATCH} CACHE STRING "" FORCE)
                set(CVLIB_SUFFIX "${OPENCV_VERSION_MAJOR}${OPENCV_VERSION_MINOR}${OPENCV_VERSION_PATCH}")
                
        endif(EXISTS "${OPENCV_DIR}/OpenCVConfig.cmake")

        
        

        ## Initiate the variable before the loop
        set(OPENCV_LIBS "")
        set(OPENCV_FOUND_TMP true)

        ## Loop over each components
        foreach(__CVLIB ${OPENCV_LIB_COMPONENTS})
                # No debug library for R@M
                #find_library(OpenCV_${__CVLIB}_LIBRARY_DEBUG NAMES "${__CVLIB}${CVLIB_SUFFIX}d" "lib${__CVLIB}${CVLIB_SUFFIX}d" PATHS "${OPENCV_DIR}/lib" NO_DEFAULT_PATH)
                find_library(OPENCV_${__CVLIB}_LIBRARY_RELEASE NAMES "${__CVLIB}${CVLIB_SUFFIX}" "lib${__CVLIB}${CVLIB_SUFFIX}" PATHS "${OPENCV_DIR}/lib" NO_DEFAULT_PATH)
                
                #Remove the cache value
                set(OPENCV_${__CVLIB}_LIBRARY "" CACHE STRING "" FORCE)
        
                #both debug/release
                if(OPENCV_${__CVLIB}_LIBRARY_DEBUG AND OPENCV_${__CVLIB}_LIBRARY_RELEASE)
                        set(OPENCV_${__CVLIB}_LIBRARY debug ${OPENCV_${__CVLIB}_LIBRARY_DEBUG} optimized ${OPENCV_${__CVLIB}_LIBRARY_RELEASE}  CACHE STRING "" FORCE)
                #only debug
                elseif(OPENCV_${__CVLIB}_LIBRARY_DEBUG)
                        set(OPENCV_${__CVLIB}_LIBRARY ${OPENCV_${__CVLIB}_LIBRARY_DEBUG}  CACHE STRING "" FORCE)
                #only release
                elseif(OPENCV_${__CVLIB}_LIBRARY_RELEASE)
                        set(OPENCV_${__CVLIB}_LIBRARY ${OPENCV_${__CVLIB}_LIBRARY_RELEASE}  CACHE STRING "" FORCE)
                #no library found
                else()
                        set(OPENCV_FOUND_TMP false)
                endif()
                
                #Add to the general list
                if(OPENCV_${__CVLIB}_LIBRARY)
                        set(OPENCV_LIBS ${OPENCV_LIBS} ${OPENCV_${__CVLIB}_LIBRARY})
                endif(OPENCV_${__CVLIB}_LIBRARY)
                
        endforeach(__CVLIB)
	    message(" KateopecvLibs: " ${OPENCV_LIBS})

        set(OPENCV_FOUND ${OPENCV_FOUND_TMP} CACHE BOOL "" FORCE)


else(EXISTS "${OPENCV_DIR}")
        set(ERR_MSG "Please specify OpenCV directory using OPENCV_DIR env. variable")
endif(EXISTS "${OPENCV_DIR}")
##====================================================


##====================================================
## Print message
##----------------------------------------------------
if(NOT OPENCV_FOUND)
  # make FIND_PACKAGE friendly
  if(NOT OPENCV_FIND_QUIETLY)
        if(OPENCV_FIND_REQUIRED)
          message(FATAL_ERROR "OpenCV required but some headers or libs not found. ${ERR_MSG}")
        else(OPENCV_FIND_REQUIRED)
          message(STATUS "WARNING: OpenCV was not found. ${ERR_MSG}")
        endif(OPENCV_FIND_REQUIRED)
  endif(NOT OPENCV_FIND_QUIETLY)
endif(NOT OPENCV_FOUND)
##====================================================


##====================================================
## Backward compatibility
##----------------------------------------------------
if(OPENCV_FOUND)
option(OPENCV_BACKWARD_COMPA "Add some variable to make this script compatible with the other version of FindOpenCV.cmake" false)
if(OPENCV_BACKWARD_COMPA)
        find_path(OPENCV_INCLUDE_DIRS "cv.h" PATHS "${OPENCV_DIR}" PATH_SUFFIXES "include" "include/opencv" DOC "Include directory") 
        find_path(OPENCV_INCLUDE_DIR "cv.h" PATHS "${OPENCV_DIR}" PATH_SUFFIXES "include" "include/opencv" DOC "Include directory")
        set(OPENCV_LIBRARIES "${OPENCV_LIBS}" CACHE STRING "" FORCE)
endif(OPENCV_BACKWARD_COMPA)
endif(OPENCV_FOUND)
##====================================================

