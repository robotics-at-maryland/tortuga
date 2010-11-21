file(GLOB HEADERS "include/*.h")
file(GLOB SOURCES "src/*.cpp")

if (NOT RAM_WITH_VISION)
  list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/PlaybackCamera.cpp")
endif ()

set(LINK_LIBS
  ram_core
  ${Boost_SERIALIZATION_LIBRARY}
  )
if (RAM_WITH_VISION)
  list(APPEND LINK_LIBS ram_vision)
endif ()
if (RAM_WITH_MATH)
  list(APPEND LINK_LIBS ram_math)
endif ()
if (RAM_WITH_VEHICLE)
  list(APPEND LINK_LIBS ram_vehicle)
endif ()
if (RAM_WITH_CONTROL)
  list(APPEND LINK_LIBS ram_control)
endif ()

if (RAM_WITH_LOGGING)
  add_library(ram_logging SHARED ${SOURCES} ${HEADERS})
  target_link_libraries(ram_logging ${LINK_LIBS})
  set_target_properties(ram_logging PROPERTIES
    DEFINE_SYMBOLS RAM_PKG_LOGGING
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )

  test_module(logging)
endif (RAM_WITH_LOGGING)
