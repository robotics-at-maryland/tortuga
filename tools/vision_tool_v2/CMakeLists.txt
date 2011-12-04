file(GLOB HEADERS "include/*.h")
file(GLOB SOURCES "src/*.cpp")

if (UNIX)
  add_definitions(-fno-strict-aliasing -Wno-deprecated-declarations)
endif (UNIX)

if (RAM_WITH_VISION_TOOLS)
  include_directories("include")
  include(${wxWidgets_USE_FILE})
  add_executable(VisionToolV2 ${SOURCES} ${HEADERS})
  target_link_libraries(VisionToolV2
    ram_vision
    ${wxWidgets_LIBRARIES}
    ${OpenCV_LIBS}
    ${OPENGL_LIBRARIES}
    )
  set_target_properties(VisionToolV2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${BINDIR}"
    )
endif (RAM_WITH_VISION_TOOLS)
