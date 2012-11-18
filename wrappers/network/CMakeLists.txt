
if (RAM_WITH_NETWORK)
  set(WRAPPED_HEADERS
    Events.h
    )

  gccxml( network "${WRAPPED_HEADERS}" )
  generate_wrappers( network )
  python_files( network )

  # test_wrapper( network "ram_network" )
endif (RAM_WITH_NETWORK)
