
set(WRAPPED_HEADERS
  Math.h
  Vector2.h
  Vector3.h
  Quaternion.h
  Matrix2.h
  Matrix3.h
  )
generate_wrappers( math "${WRAPPED_HEADERS}" )
