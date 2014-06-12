
if (RAM_WITH_CONTROL)
  set(WRAPPED_HEADERS
    IController.h
    DesiredState.h
    Helpers.h
    )
  if (RAM_WITH_CORE)
    list(APPEND WRAPPED_HEADERS Events.h)
  endif (RAM_WITH_CORE)

  gccxml( control "${WRAPPED_HEADERS}" )
  generate_wrappers( control )
  python_files( control )

  test_wrapper( control "ram_control" )
endif (RAM_WITH_CONTROL)
