file(GLOB HEADERS "include/*.h")
file(GLOB SOURCES "src/*.cpp")

set(LINK_LIBS
  ${Boost_THREAD_LIBRARY}
  )

if (RAM_WITH_PATTERN)
  add_library(ram_pattern SHARED ${SOURCES} ${HEADERS})
  target_link_libraries(ram_pattern ${LINK_LIBS})
  set_target_properties(ram_pattern PROPERTIES
    DEFINE_SYMBOL RAM_PKG_PATTERN
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )

  test_module(pattern "ram_pattern")
endif (RAM_WITH_PATTERN)
