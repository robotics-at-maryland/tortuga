
option(RAM_TESTS "Build and run unittests" ON)

# Optional argument for an exclude list
macro(test_module _name)
  if (RAM_TESTS)
    string(TOUPPER ${_name} _module_name)
    file(GLOB TEST_${_module_name}_SOURCES "test/src/*.cxx")

    foreach (_file ${ARGV})
      list(REMOVE_ITEM TEST_${_module_name}_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/${_file})
    endforeach ()
    
    add_executable(Tests_${_name} ${TEST_${_module_name}_SOURCES})
    target_link_libraries(Tests_${_name}
      ram_${_name}
      ${UnitTest++_LIB_DIR}
      )

    add_custom_command(
      OUTPUT Tests_${_name}.success
      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/Tests_${_name}
      MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/Tests_${_name}
      )
    add_custom_target(ram_${_name}_tests ALL DEPENDS Tests_${_name}.success)
  endif (RAM_TESTS)
endmacro ()
