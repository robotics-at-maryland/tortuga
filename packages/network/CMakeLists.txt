file(GLOB HEADERS "include/*.h")
file(GLOB SOURCES "src/*.cpp")

set(LINK_LIBS
  ${Boost_SYSTEM_LIBRARY}
  ram_core
  ram_control
  )

if (RAM_WITH_NETWORK)
  add_library(ram_network SHARED ${SOURCES} ${HEADERS})
  target_link_libraries(ram_network ${LINK_LIBS})
  set_target_properties(ram_network PROPERTIES
    DEFINE_SYMBOL RAM_PKG_NETWORK
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )
endif (RAM_WITH_NETWORK)
