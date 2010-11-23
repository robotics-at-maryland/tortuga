
file(GLOB HEADERS "include/*.h")
file(GLOB SOURCES "src/*.cpp")

if (RAM_WITH_VISION_VIEWER)
  include_directories("include")
  include(${wxWidgets_USE_FILE})

  add_definitions(-fno-strict-aliasing)
  add_executable(Viewer ${SOURCES} ${HEADERS})
  target_link_libraries(Viewer
    ram_vision
    ${wxWidgets_LIBRARIES}
    )
endif (RAM_WITH_VISION_VIEWER)
