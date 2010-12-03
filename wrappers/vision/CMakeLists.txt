
if (RAM_WITH_VISION)
  set(WRAPPED_HEADERS
    Events.h
    VisionSystem.h
    )

  gccxml( vision "${WRAPPED_HEADERS}" )
  generate_wrappers( vision )
  python_files( vision )

  test_wrapper( vision "ram_vision" )
endif (RAM_WITH_VISION)
