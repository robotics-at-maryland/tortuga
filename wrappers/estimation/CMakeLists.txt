
if (RAM_WITH_ESTIMATION)
  set(WRAPPED_HEADERS
    IStateEstimator.h
    )

  gccxml( estimation "${WRAPPED_HEADERS}" )
  generate_wrappers( estimation )
  python_files( estimation )

  test_wrapper( estimation "ram_estimation" )
endif (RAM_WITH_ESTIMATION)