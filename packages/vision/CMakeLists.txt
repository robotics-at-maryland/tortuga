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
  ${FANN_LIBRARY}
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

  add_executable(LCHLookupTable "test/src/LCHLookupTable.cpp")
  target_link_libraries(LCHLookupTable
    ram_vision
    )

  set(VISION_EXCLUDE_LIST "test/src/TestConvert.cxx")
  test_module(vision "ram_vision")
endif (RAM_WITH_VISION)
