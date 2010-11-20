
option(RAM_WITH_CORE "Build core module" ON)

macro (add_feature _name)
  string(TOUPPER ${_name} NAME)
  option(RAM_WITH_${NAME} "Build ${_name} module" ON)

  if (RAM_WITH_${NAME})
    file(APPEND ${FEATURE_HEADER} "#define RAM_WITH_${NAME}\n")
  endif ()
endmacro ()

set(FEATURE_HEADER ${CMAKE_SOURCE_DIR}/packages/core/include/Feature.h)
file(WRITE ${FEATURE_HEADER} "// DO NOT EDIT, This is a generated header\n")
file(APPEND ${FEATURE_HEADER} "#ifndef RAM_CORE_FEATURES\n")
file(APPEND ${FEATURE_HEADER} "#define RAM_CORE_FEATURES\n\n")

add_feature(pattern)
add_feature(drivers)
add_feature(core)
add_feature(math)
add_feature(vehicle)

file(APPEND ${FEATURE_HEADER} "\n#endif // RAM_CORE_FEATURES\n")
message(STATUS "Features header file generated")
