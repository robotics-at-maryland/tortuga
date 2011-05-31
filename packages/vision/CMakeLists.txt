file(GLOB HEADERS "include/*.h")
file(GLOB SOURCES "src/*.cpp")

set(LINK_LIBS
  ram_pattern
  ram_core
  ram_math
  ${OpenCV_LIBS}
  ${Boost_THREAD_LIBRARY}
  ${Boost_REGEX_LIBRARY}
  ${DC1394_LIBRARIES}
  ${FANN_LIBRARIES}
  ${FFTW_LIBRARY}
  )

if (RAM_WITH_VISION)
  add_library(ram_vision SHARED ${SOURCES} ${HEADERS})
  target_link_libraries(ram_vision ${LINK_LIBS})

  set_target_properties(ram_vision PROPERTIES
    DEFINE_SYMBOL RAM_PKG_VISION
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )

  add_executable(OpticalFlowTest "test/src/OpticalFlow.cpp")
  target_link_libraries(OpticalFlowTest
    ${OpenCV_LIBS}
    )

  add_executable(ImageAnalyzer "test/src/ImageAnalysisTest.cpp")
  target_link_libraries(ImageAnalyzer
    ram_vision
    )

  add_executable(SuitHistoCalculator "test/src/SuitHistoCalculatorTest.cpp")
  target_link_libraries(SuitHistoCalculator
    ram_vision
    )

  add_executable(DetectorTest "test/src/DetectorTest.cpp")
  target_link_libraries(DetectorTest
    ram_vision
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    )
  if (RAM_WITH_LOGGING)
    target_link_libraries(DetectorTest
      ram_logging
      ram_math
      ram_control
      )
  endif (RAM_WITH_LOGGING)

  add_executable(UndistortTest "test/src/UndistortTest.cpp")
  target_link_libraries(UndistortTest
    ram_vision
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    )

  add_executable(NetworkTrainer "test/src/NetworkTrainer.cpp")
  target_link_libraries(NetworkTrainer
    ram_vision
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    )

  add_executable(PhaseCorrelation "test/src/phase_correlation.cpp")
  target_link_libraries(PhaseCorrelation
    ram_vision
    ${FFTW_LIBRARY}
    )

  add_executable(FeatureAnalyzer "test/src/FeatureAnalyzer.cpp")
  target_link_libraries(FeatureAnalyzer
    ram_vision
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    )

  add_executable(LCHLookupTable "test/src/LCHLookupTable.cpp")
  target_link_libraries(LCHLookupTable
    ram_vision
    )

  set(vision_EXCLUDE_LIST "test/src/TestConvert.cxx")
  test_module(vision "ram_vision")
endif (RAM_WITH_VISION)
