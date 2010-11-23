
set(AVAILABLE_FEATURES)
macro (feature _name)
  string(TOUPPER ${_name} NAME)
  option(RAM_WITH_${NAME} "Build ${_name} module" ON)

  if (RAM_WITH_${NAME})
    list(APPEND AVAILABLE_FEATURES "${_name}")
  endif ()
endmacro ()

macro (generate_feature_list)
  set(FEATURE_HEADER ${CMAKE_SOURCE_DIR}/packages/core/include/Feature.h)
  file(WRITE ${FEATURE_HEADER} "// DO NOT EDIT, This is a generated header\n")
  file(APPEND ${FEATURE_HEADER} "#ifndef RAM_CORE_FEATURES\n")
  file(APPEND ${FEATURE_HEADER} "#define RAM_CORE_FEATURES\n\n")
  
  foreach (_name ${AVAILABLE_FEATURES})
    string(TOUPPER ${_name} NAME)
    file(APPEND ${FEATURE_HEADER} "#define RAM_WITH_${NAME}\n")
  endforeach ()
  
  file(APPEND ${FEATURE_HEADER} "\n#endif // RAM_CORE_FEATURES\n")
  message(STATUS "Features header file generated")
endmacro ()

macro (check_features)
  # Check if the features have changed, regenerate file if they have
  string(REPLACE ";" "," FEATURES "${AVAILABLE_FEATURES}")
  message(STATUS "Features: ${FEATURES}")

  list(LENGTH AVAILABLE_FEATURES _new_length)
  list(LENGTH RAM_FEATURES _old_length)

  # Only check contents if they're both the same size
  if (${_new_length} EQUAL ${_old_length})
    set(GENERATE_HEADER NO)
    foreach ( _feature ${AVAILABLE_FEATURES} )
      list(FIND RAM_FEATURES ${_feature} FOUND)
      if (${FOUND} EQUAL -1)
        set(GENERATE_HEADER YES)
        break ()
      endif ()
    endforeach ()
  else ()
    # Different size list, features must be different
    set(GENERATE_HEADER YES)
  endif ()

  if (GENERATE_HEADER)
    generate_feature_list()
  endif ()

  # Set current feature list in cache as persistent variable
  set(RAM_FEATURES ${AVAILABLE_FEATURES} CACHE INTERNAL "Feature List")
endmacro ()

feature(pattern)
feature(drivers)
feature(core)
feature(math)
feature(vehicle)
feature(control)
feature(vision)
feature(network)
feature(logging)
feature(wrappers)
feature(vision_tools)

check_features()
