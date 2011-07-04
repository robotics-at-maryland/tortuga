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

  # Blackfin programs
  if (BLACKFIN)
    # sonar daemon program
    set(SONARD
      src/sonard/main.cpp
      src/sonard/trigger.c
      src/sonard/trigger.h
      )
    add_executable(sonard ${SONARD})
    target_link_libraries(sonard ram_sonar)
    set_target_properties(sonard PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${BINDIR}")

    # rtspectrogram
    add_executable(rtspectrogram src/tools/rtspectrogram.cpp)
    target_link_libraries(rtspectrogram ram_sonar)
    set_target_properties(rtspectrogram PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${BINDIR}")

    add_executable(chop src/tools/chop.cpp)
    target_link_libraries(chop ram_sonar)
    set_target_properties(chop PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${BINDIR}")
  endif (BLACKFIN)
endif (RAM_WITH_SONAR)
