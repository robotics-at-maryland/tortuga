
set(WRAPPED_HEADERS
  Math.h
  Vector2.h
  Vector3.h
  Quaternion.h
  Matrix2.h
  Matrix3.h
  )
gccxml( math "${WRAPPED_HEADERS}" )
generate_wrappers( math )
python_files( math )
