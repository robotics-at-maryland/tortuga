file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

set(LINK_LIBS
  ram_core
  ram_math
  ram_vehicle
  ram_vision
  )

if (RAM_WITH_ESTIMATION)
  add_library(ram_estimation SHARED ${SOURCES} ${HEADERS})
  target_link_libraries(ram_estimation ${LINK_LIBS})
  set_target_properties(ram_estimation PROPERTIES
    DEFINE_SYMBOL RAM_PKG_ESTIMATION
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )

  test_module(estimation "ram_estimation")
endif (RAM_WITH_ESTIMATION)
