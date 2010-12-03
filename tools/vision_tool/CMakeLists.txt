
file(GLOB HEADERS "include/*.h")
file(GLOB SOURCES "src/*.cpp")

if (UNIX)
  add_definitions(-fno-strict-aliasing -Wno-deprecated-declarations)
endif (UNIX)

if (RAM_WITH_VISION_TOOLS)
  include_directories("include")
  include(${wxWidgets_USE_FILE})
  add_executable(VisionTool ${SOURCES} ${HEADERS})
  target_link_libraries(VisionTool
    ram_vision
    ${wxWidgets_LIBRARIES}
    ${OpenCV_LIBS}
    )
  set_target_properties(VisionTool PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${BINDIR}"
    )
endif (RAM_WITH_VISION_TOOLS)
