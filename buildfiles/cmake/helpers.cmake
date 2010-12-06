
set(RAM_TESTS ON CACHE BOOL "Build and run unittests")
set(RAM_CONTINUOUS_INTEGRATION ON CACHE BOOL "Run unittests during the build")
if (RAM_TESTS)
  enable_testing()
endif (RAM_TESTS)

macro(test_module _name _link_libs)
  if (RAM_TESTS)
    test_module_base(${_name} "${_link_libs}" ${ARGV})
    if (RAM_CONTINUOUS_INTEGRATION)
      add_custom_target(ram_${_name}_tests ALL DEPENDS Tests_${_name}.success)
    endif (RAM_CONTINUOUS_INTEGRATION)
  endif (RAM_TESTS)
endmacro ()

macro(test_wrapper _name _link_libs)
  if (RAM_TESTS)
    set(WRAPPER_TEST_DEPENDS)
    if (DEFINED PYTHON_${_name}_FILELIST)
      list(APPEND WRAPPER_TEST_DEPENDS ${PYTHON_${_name}_FILELIST})
    endif ()

    # Run macro to set up C++ tests
    set(${_name}_wrapper_DEPENDENCIES ${WRAPPER_TEST_DEPENDS})
    test_module_base(${_name}_wrapper "${_link_libs}" ${ARGV})

    if (RAM_CONTINUOUS_INTEGRATION)
      add_custom_target(ram_${_name}_wrapper_tests ALL DEPENDS
        ${WRAPPER_TEST_DEPENDS})
    endif (RAM_CONTINUOUS_INTEGRATION)

    list(APPEND WRAPPER_TEST_DEPENDS Tests_${_name}_wrapper.success _${_name})

    # Glob all python files
    file(GLOB ${_name}_PYTESTS "${_directory}/*.py")
    add_custom_command(
      OUTPUT ${CMAKE_SOURCE_DIR}/build_ext/ext/_${_name}Tests.success
      COMMAND ${PYTHON_EXECUTABLE}
      ARGS "scripts/pytester.py" ${${_name}_PYTESTS}
      COMMAND ${CMAKE_COMMAND} -E touch
      ARGS build_ext/ext/_${_name}Tests.success
      DEPENDS _${_name} ${WRAPPER_TEST_DEPENDS}
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      )
    add_test(python_${_name}
      python ${CMAKE_SOURCE_DIR}/scripts/pytester.py ${${_name}_PYTESTS})
    if (RAM_CONTINUOUS_INTEGRATION)
      add_custom_target(${_name}_pywrapper_tests ALL DEPENDS
        ${CMAKE_SOURCE_DIR}/build_ext/ext/_${_name}Tests.success)
    endif (RAM_CONTINUOUS_INTEGRATION)
  endif (RAM_TESTS)
endmacro ()

# This macro changes behavior for the following defined values
# _target_EXCLUDE_LIST will exclude sources that were found in a glob
#     from the compilation.
# _target_DEPENDENCIES allows you to specify additional dependencies
#     before a test gets run. Compilation dependencies are determined
#     automatically.
# _target_DIRECTORY overrides the default glob directory. The default
#     glob directory is test/src.
macro(test_module_base _target _link_libs)
  if (RAM_TESTS)
    if (DEFINED ${_target}_DIRECTORY)
      set(_directory ${${_target}_DIRECTORY})   
    else ()
      set(_directory "test/src")
    endif ()
    file(GLOB TEST_${_target}_SOURCES "${_directory}/*.cxx")

    if (DEFINED ${_target}_EXCLUDE_LIST)
      foreach (_file ${${_target}_EXCLUDE_LIST})
        list(REMOVE_ITEM TEST_${_target}_SOURCES
          ${CMAKE_CURRENT_SOURCE_DIR}/${_file})
      endforeach ()
    endif ()
    
    add_executable(Tests_${_target} ${TEST_${_target}_SOURCES})
    target_link_libraries(Tests_${_target}
      ${_link_libs}
      ${UnitTest++_LIB_DIR}
      )

    add_test(Tests_${_target} Tests_${_target})
    add_custom_command(
      OUTPUT Tests_${_target}.success
      COMMAND Tests_${_target}
      COMMAND ${CMAKE_COMMAND} -E touch Tests_${_target}.success
      DEPENDS Tests_${_target} ${${_target}_DEPENDENCIES}
      )
  endif (RAM_TESTS)
endmacro ()
