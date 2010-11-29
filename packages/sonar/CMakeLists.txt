file(GLOB HEADERS "include/*.h")
file(GLOB SOURCES "src/*.cpp")
file(GLOB SONARD "src/sonard/*.c")
file(GLOB FIXED "src/fixed/*.cpp")

set(LINK_LIBS
  ram_math
  ram_bfin_spartan
  )

if (NOT BLACKFIN)
  list(APPEND LINK_LIBS ${FFTW_LIBRARY})
endif (NOT BLACKFIN)

if (RAM_WITH_SONAR)
  include_directories("include")
  add_library(ram_sonar SHARED ${SOURCES} ${HEADERS} ${SONARD} ${FIXED})
  target_link_libraries(ram_sonar ${LINK_LIBS})
  set_target_properties(ram_sonar PROPERTIES
    DEFINE_SYMBOL RAM_PKG_SONAR
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )

  # Tests
  add_executable(testGetChunk "test/src/TestGetChunk.cxx")
  target_link_libraries(testGetChunk ram_sonar)

  add_executable(testGetDirEdge "test/src/TestGetDirEdge.cxx")
  target_link_libraries(testGetDirEdge ram_sonar)

  add_executable(testPingDetect "test/src/TestPingDetect.cxx")
  target_link_libraries(testPingDetect ram_sonar)
endif (RAM_WITH_SONAR)
