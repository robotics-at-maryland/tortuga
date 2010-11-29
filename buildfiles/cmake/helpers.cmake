
set(RAM_TESTS ON CACHE BOOL "Build and run unittests")

macro(test_module _name _link_libs)
  test_module_base(${_name} "${_link_libs}" ${ARGV})

  if (RAM_TESTS)
    add_custom_target(ram_${_name}_tests ALL DEPENDS Tests_${_name}.success)
  endif (RAM_TESTS)
endmacro ()

macro(test_wrapper _name _link_libs)
  test_module_base(${_name}_wrapper "${_link_libs}" ${ARGV})

  if (RAM_TESTS)
    set(WRAPPER_TEST_DEPENDS Tests_${_name}_wrapper.success _${_name})
    if (DEFINED PYTHON_${_name}_FILELIST)
      list(APPEND WRAPPER_TEST_DEPENDS ${PYTHON_${_name}_FILELIST})
    endif ()
    add_custom_target(ram_${_name}_wrapper_tests ALL DEPENDS
      ${WRAPPER_TEST_DEPENDS})
  endif (RAM_TESTS)

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
  add_custom_target(${_name}_pywrapper_tests ALL DEPENDS
    ${CMAKE_SOURCE_DIR}/build_ext/ext/_${_name}Tests.success)
endmacro ()

# Optional argument for an exclude list
macro(test_module_base _target _link_libs)
  if (RAM_TESTS)
    string(TOUPPER ${_target} _module_name)
    if (DEFINED ${_module_name}_DIRECTORY)
      set(_directory ${${_module_name}_DIRECTORY})   
    else ()
      set(_directory "test/src")
    endif ()
    file(GLOB TEST_${_module_name}_SOURCES "${_directory}/*.cxx")

    if (DEFINED ${_module_name}_EXCLUDE_LIST)
      foreach (_file ${${_module_name}_EXCLUDE_LIST})
        list(REMOVE_ITEM TEST_${_module_name}_SOURCES
          ${CMAKE_CURRENT_SOURCE_DIR}/${_file})
      endforeach ()
    endif ()
    
    add_executable(Tests_${_target} ${TEST_${_module_name}_SOURCES})
    target_link_libraries(Tests_${_target}
      ${_link_libs}
      ${UnitTest++_LIB_DIR}
      )

    add_custom_command(
      OUTPUT Tests_${_target}.success
      COMMAND Tests_${_target}
      COMMAND ${CMAKE_COMMAND} -E touch Tests_${_target}.success
      DEPENDS Tests_${_target}
      )
  endif (RAM_TESTS)
endmacro ()
