
if (RAM_TESTS)
  find_package(UnitTest++ REQUIRED)
  include_directories(${UnitTest++_INCLUDE_DIR})
endif (RAM_TESTS)
