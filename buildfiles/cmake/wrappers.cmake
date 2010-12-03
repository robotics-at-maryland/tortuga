
find_package(PythonLibs)
find_package(PythonInterp)
include_directories(${PYTHON_INCLUDE_PATH})
set(WRAPPER_DIR ${CMAKE_SOURCE_DIR}/build_ext)

find_package(GCCXML)
set(ENV{PYTHONPATH} ${CMAKE_SOURCE_DIR} $ENV{PYTHONPATH})

macro(gccxml MODULE HEADERS)
  set(DIRECTORY "${CMAKE_SOURCE_DIR}/packages/${MODULE}/include")
  set (XMLFILES "")
  set (GCCXML_FLAGS "-I${CMAKE_SOURCE_DIR}/packages" "-I${CMAKE_SOURCE_DIR}" "-I${PYTHON_INCLUDE_PATH}" "-I${RAM_ROOT_DIR}/include" "-I${CMAKE_SOURCE_DIR}/packages/${MODULE}" "-DRAM_POSIX" "-DRAM_LINUX" "-DBOOST_PYTHON_NO_PY_SIGNATURES")

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

# Generates the source code for the wrappers.
# The gccxml macro MUST be called before this macro.
macro(pypp MODULE)
  set (GEN_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/_${MODULE}_gen-sources.txt.cache)
  set (PYPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/generated/_${MODULE}.main.cpp)
  make_directory(generated)
  add_custom_command(
    OUTPUT ${PYPP_FILE}
    COMMAND ${PYTHON_EXECUTABLE}
    ARGS "scripts/pypp.py" "-t" "${GEN_SOURCES}" "-m" "_${MODULE}" "${CMAKE_CURRENT_SOURCE_DIR}/gen_${MODULE}.py" ${XMLFILES}
    DEPENDS ${XMLFILES} ${CMAKE_CURRENT_SOURCE_DIR}/gen_${MODULE}.py
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endmacro ()

make_directory(${CMAKE_SOURCE_DIR}/build_ext/ext)

# Calls pypp to generate the source code files and then
# compiles them. You MUST call the gccxml macro before this.
macro(generate_wrappers MODULE)
  pypp( ${MODULE} )

  set(_dir_extension)
  if (DEFINED ${MODULE}_WRAPPER_DIRECTORY_EXT)
    set(_dir_extension ${${MODULE}_WRAPPER_DIRECTORY_EXT})
  endif ()
  file(GLOB ${MODULE}_WRAPPER_HEADERS "include/${_dir_extension}/*.h")
  file(GLOB ${MODULE}_WRAPPER_SOURCES "src/${_dir_extension}/*.cpp")

  add_definitions(-fno-strict-aliasing)
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated ${CMAKE_SOURCE_DIR})
  add_library(_${MODULE} SHARED
    ${PYPP_FILE}
    ${${MODULE}_WRAPPER_HEADERS}
    ${${MODULE}_WRAPPER_SOURCES}
    )

  # If an alternative base has been set,
  # use it instead of trying to figure it out ourselves
  if (DEFINED ${MODULE}_WRAPPER_BASE)
    target_link_libraries(_${MODULE} ${${MODULE}_WRAPPER_BASE})
  else ()
    target_link_libraries(_${MODULE} ram_${MODULE})
  endif ()
  target_link_libraries(_${MODULE}
    ${Boost_PYTHON_LIBRARY}
    ${PYTHON_LIBRARIES}
    )
  set_target_properties(_${MODULE} PROPERTIES
    PREFIX ""
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build_ext/ext
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build_ext/ext
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build_ext/ext
    )
  add_custom_target(ram_${MODULE}_wrapper ALL DEPENDS _${MODULE})
endmacro ()

set(EXT_SOURCE ${CMAKE_SOURCE_DIR}/wrappers/ext_init.py)
set(EXT_INIT_FILE ${CMAKE_SOURCE_DIR}/build_ext/ext/__init__.py)
add_custom_command(
  OUTPUT ${EXT_INIT_FILE}
  COMMAND ${CMAKE_COMMAND} -E copy
  ARGS ${EXT_SOURCE} ${EXT_INIT_FILE}
  DEPENDS ${EXT_SOURCE}
  COMMENT "Copying ${EXT_SOURCE} to ${EXT_INIT_FILE}"
  )
add_custom_target(ram_ext_init ALL DEPENDS ${EXT_INIT_FILE})

macro(python_files MODULE)
  file(GLOB_RECURSE PYTHON_FILES "python/*.py")

  set(PYTHON_${MODULE}_FILELIST)
  foreach (PYFILE ${PYTHON_FILES})
    string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/python "" BASEDIR ${PYFILE})
    set(OUTPUT_FILE ${CMAKE_SOURCE_DIR}/build_ext/ext${BASEDIR})
    add_custom_command(
      OUTPUT ${OUTPUT_FILE}
      COMMAND ${CMAKE_COMMAND} -E copy
      ARGS ${PYFILE} ${OUTPUT_FILE}
      DEPENDS ${PYFILE} ${EXT_INIT_FILE}
      COMMENT "Copying ${PYFILE} to ${OUTPUT_FILE}"
      )
    list(APPEND PYTHON_${MODULE}_FILELIST ${OUTPUT_FILE})
  endforeach ()
  add_custom_target(ram_${MODULE}_python ALL DEPENDS
    ${PYTHON_${MODULE}_FILELIST})
endmacro ()
