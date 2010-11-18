
find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})
set(WRAPPER_DIR ${CMAKE_SOURCE_DIR}/build_ext)

find_package(GCCXML)

macro(gccxml MODULE HEADERS)
  set(DIRECTORY "${CMAKE_SOURCE_DIR}/packages/${MODULE}/include")
  make_directory(generated)
  set (XMLFILES "")
  set (GCCXML_FLAGS "-I${CMAKE_SOURCE_DIR}/packages" "-I${CMAKE_SOURCE_DIR}" "-I/opt/ram/local/include/boost-1_37" "-I${PYTHON_INCLUDE_PATH}" "-I/opt/ram/local/include" "-I${CMAKE_SOURCE_DIR}/packages/${MODULE}" "-DRAM_POSIX" "-DRAM_LINUX" "-DBOOST_PYTHON_NO_PY_SIGNATURES")
  if (UNIX)
    set(GCCXML_FLAGS ${GCCXML_FLAGS} "--gccxml-compiler" "gcc-4.2")
  endif (UNIX)

  foreach (HEADER ${HEADERS})
    string (REGEX REPLACE "\\.h$" ".xml" XMLNAME ${HEADER})
    set (XMLOUTPUT generated/${XMLNAME})

    add_custom_command(
      OUTPUT ${XMLOUTPUT}
      COMMAND ${GCCXML}
      ARGS ${DIRECTORY}/${HEADER} ${GCCXML_FLAGS} -fxml=${XMLOUTPUT}
      MAIN_DEPENDENCY ${DIRECTORY}/${HEADER}
      )
    set (XMLFILES ${XMLFILES} ${XMLOUTPUT})
  endforeach ()
  add_custom_target(ram_${MODULE}_wrapper ALL DEPENDS ${XMLFILES})
endmacro ()
