
find_package(PythonLibs)
find_package(PythonInterp)
include_directories(${PYTHON_INCLUDE_PATH})
set(WRAPPER_DIR ${CMAKE_SOURCE_DIR}/build_ext)

find_package(GCCXML)
set(ENV{PYTHONPATH} ${CMAKE_SOURCE_DIR} $ENV{PYTHONPATH})

macro(gccxml MODULE HEADERS)
  set(DIRECTORY "${CMAKE_SOURCE_DIR}/packages/${MODULE}/include")
  set (XMLFILES "")
  set (GCCXML_FLAGS "-I${CMAKE_SOURCE_DIR}/packages" "-I${CMAKE_SOURCE_DIR}" "-I/opt/ocm/local/include" "-I${PYTHON_INCLUDE_PATH}" "-I/opt/ram/local/include" "-I${CMAKE_SOURCE_DIR}/packages/${MODULE}" "-DRAM_POSIX" "-DRAM_LINUX" "-DBOOST_PYTHON_NO_PY_SIGNATURES")

  foreach (HEADER ${HEADERS})
    string (REGEX REPLACE "\\.h$" ".xml" XMLNAME ${HEADER})
    set(XMLOUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${XMLNAME})

    add_custom_command(
      OUTPUT ${XMLOUTPUT}
      COMMAND ${GCCXML}
      ARGS ${DIRECTORY}/${HEADER} ${GCCXML_FLAGS} -fxml=${XMLOUTPUT}
      MAIN_DEPENDENCY ${DIRECTORY}/${HEADER}
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      )
    set (XMLFILES ${XMLFILES} ${XMLOUTPUT})
  endforeach ()
endmacro ()

macro(pypp MODULE HEADERS)
  gccxml( ${MODULE} "${HEADERS}" )
  set (GEN_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/_${MODULE}_gen-sources.txt.cache)
  set (PYPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/generated/_${MODULE}.main.cpp)
  make_directory(generated)
  add_custom_command(
    OUTPUT ${PYPP_FILE}
    COMMAND "PYTHONPATH=${CMAKE_SOURCE_DIR}" ${PYTHON_EXECUTABLE}
    ARGS "scripts/pypp.py" "-t" "${GEN_SOURCES}" "-m" "_${MODULE}" "${CMAKE_CURRENT_SOURCE_DIR}/gen_${MODULE}.py" ${XMLFILES}
    DEPENDS ${XMLFILES} ${CMAKE_CURRENT_SOURCE_DIR}/gen_${MODULE}.py
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endmacro ()

make_directory(${CMAKE_SOURCE_DIR}/build_ext/ext)
file(WRITE ${CMAKE_SOURCE_DIR}/build_ext/ext/__init__.py "")
macro(generate_wrappers MODULE HEADERS)
  pypp( ${MODULE} "${HEADERS}" )

  file(GLOB ${MODULE}_WRAPPER_HEADERS "include/*.h")
  file(GLOB ${MODULE}_WRAPPER_SOURCES "src/*.cpp")

  add_definitions(-fno-strict-aliasing)
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated ${CMAKE_SOURCE_DIR})
  add_library(_${MODULE} SHARED
    ${PYPP_FILE}
    ${${MODULE}_WRAPPER_HEADERS}
    ${${MODULE}_WRAPPER_SOURCES}
    )
  set_target_properties(_${MODULE} PROPERTIES
    PREFIX ""
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build_ext/ext
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build_ext/ext
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build_ext/ext
    )
  target_link_libraries(_${MODULE}
    ram_${MODULE}
    ${Boost_PYTHON_LIBRARY}
    ${PYTHON_LIBRARIES}
    )

  set(EXT_${MODULE} ${CMAKE_SOURCE_DIR}/build_ext/ext/${MODULE}.py)
  add_custom_command(
    OUTPUT ${EXT_${MODULE}}
    COMMAND ${CMAKE_COMMAND} -E copy
    ARGS python/${MODULE}.py ${EXT_${MODULE}}
    DEPENDS python/${MODULE}.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Copying ${CMAKE_CURRENT_SOURCE_DIR}/python/${MODULE}.py to ${CMAKE_SOURCE_DIR}/build_ext/ext"
    )
  add_custom_target(ram_${MODULE}_wrapper ALL DEPENDS ${EXT_${MODULE}} _${MODULE})
endmacro ()
