
macro(add_feature _name)
  string(TOUPPER ${_name} _feature_name)
  option(RAM_WITH_${_feature_name} "Build with ${_feature_name} feature" ON)
  if (RAM_WITH_${_feature_name})
    set(RAM_FEATURES
      ${RAM_FEATURES}
      ${_feature_name}
      )
  endif ()
endmacro ()