file(GLOB HEADERS "include/*.h")
file(GLOB SOURCES "src/*.cpp")

set(LINK_LIBS
  pthread
  ${Boost_THREAD_LIBRARY}
  ram_core
  ram_control
  ${ZeroCIceCore_LIBRARY}
  ${ZeroCIceUtil_LIBRARY}
  )

if (RAM_WITH_LOGGING)
  list(APPEND LINK_LIBS ram_logging)
endif (RAM_WITH_LOGGING)

if (RAM_WITH_NETWORK)
  # Run slice file generation programs
  slice( network )

  # Include the generated headers from slice
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/slice)
  add_library(ram_network SHARED ${SOURCES} ${HEADERS} ${network_SLICE_SOURCES})
  target_link_libraries(ram_network ${LINK_LIBS})
  set_target_properties(ram_network PROPERTIES
    DEFINE_SYMBOL RAM_PKG_NETWORK
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )

  add_executable(EventListener "test/src/EventListener.cpp")
  target_link_libraries(EventListener ram_network)

  test_module(network "ram_network")
endif (RAM_WITH_NETWORK)
