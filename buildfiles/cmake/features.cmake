
set(AVAILABLE_FEATURES)
macro (feature _name)
  # Create the cache entry for this feature
  string(TOUPPER ${_name} NAME)
  set(RAM_WITH_${NAME} ON CACHE BOOL "Build ${_name} module")

  # If a DEPENDS list has been given, make sure all those features exist
  set(ARGUMENTS ${ARGN})

  # Check if we're building for the blackfin
  if (BLACKFIN)
    # Feature must include "BFIN" parameter to be considered
    list(FIND ARGUMENTS "BFIN" BFIN_LOC)
    if (${BFIN_LOC} EQUAL -1)
      # Turn off the feature (it should not be built on the blackfin)
      set(RAM_WITH_${NAME} OFF CACHE BOOL "Build ${_name} module" FORCE)
    endif ()
  endif (BLACKFIN)

  list(FIND ARGUMENTS "DEPENDS" DEPENDS_LOC)
  if (${DEPENDS_LOC} GREATER -1)
    set(INDEX ${DEPENDS_LOC})
    math(EXPR INDEX "1+${INDEX}")
    list(LENGTH ARGUMENTS LENGTH)

    # Prove that the feature is not available
    while (${INDEX} LESS ${LENGTH})
      list(GET ARGUMENTS ${INDEX} FEATURE_NAME)
      string(TOUPPER ${FEATURE_NAME} FEATURE_NAME)

      if (NOT RAM_WITH_${FEATURE_NAME})
        # Feature is not available as a dependency is not on
        set(RAM_WITH_${NAME} OFF CACHE BOOL "Build ${_name} module" FORCE)
      endif ()
      math(EXPR INDEX "1+${INDEX}")
    endwhile ()
  endif ()

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
feature(math BFIN)
feature(vehicle DEPENDS math core pattern)
feature(control DEPENDS math core vehicle)
feature(vision DEPENDS pattern core math)
feature(network DEPENDS core)
feature(logging DEPENDS core)
feature(wrappers DEPENDS core)
feature(sonar BFIN DEPENDS math)
feature(vision_tools DEPENDS vision)
feature(vision_viewer DEPENDS vision)

check_features()
