
if (NOT BLACKFIN)
  #set(CMAKE_FIND_ROOT_PATH ${RAM_ROOT_DIR})
  find_package(wxWidgets REQUIRED COMPONENTS std gl aui)
  include_directories(${wxWidgets_INCLUDE_DIRS})
endif ()
