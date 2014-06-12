file(GLOB HEADERS "include/*.h")
file(GLOB SOURCES "src/*.cpp")

set(LINK_LIBS
  ram_core
  ram_math
  ram_vehicle
  ram_estimation
  )

if (RAM_WITH_CONTROL)
  add_library(ram_control SHARED ${SOURCES} ${HEADERS})
  target_link_libraries(ram_control ${LINK_LIBS})
  set_target_properties(ram_control PROPERTIES
    DEFINE_SYMBOL RAM_PKG_CONTROL
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )

  test_module(control "ram_control")
endif (RAM_WITH_CONTROL)
