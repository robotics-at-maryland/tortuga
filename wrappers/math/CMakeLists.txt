
if (RAM_WITH_MATH)
  set(WRAPPED_HEADERS
    Math.h
    Vector2.h
    Vector3.h
    Quaternion.h
    Matrix2.h
    Matrix3.h
    )
  if (RAM_WITH_CORE)
    list(APPEND WRAPPED_HEADERS Events.h)
  endif (RAM_WITH_CORE)

  gccxml( math "${WRAPPED_HEADERS}" )
  generate_wrappers( math )
  python_files( math )

  test_wrapper( math "ram_math" )
endif (RAM_WITH_MATH)
