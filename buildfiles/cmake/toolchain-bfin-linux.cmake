CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# --------------------------------------------------------------------------- #
#              T O O L C H A I N   C O N F I G U R A T I O N                  #
# --------------------------------------------------------------------------- #

# We are using linux and compiling for the blackfin
SET(CMAKE_SYSTEM_NAME Linux)
SET(BLACKFIN TRUE)
ADD_DEFINITIONS(-DBLACKFIN)

# Specify the cross compiler
SET(CMAKE_C_COMPILER /opt/uClinux/bfin-linux-uclibc/bin/bfin-linux-uclibc-gcc)
SET(CMAKE_CXX_COMPILER /opt/uClinux/bfin-linux-uclibc/bin/bfin-linux-uclibc-g++)

# Force tests off
SET(RAM_TESTS OFF CACHE BOOL "Build and run unittests" FORCE)
