
find_package(ZeroCIce)
if (ZeroCIce_FOUND)
  include_directories(${ZeroCIce_INCLUDE_DIR})
endif (ZeroCIce_FOUND)