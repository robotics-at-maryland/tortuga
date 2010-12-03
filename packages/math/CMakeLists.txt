file(GLOB HEADERS "include/*.h")
file(GLOB SOURCES "src/*.cpp")

if (RAM_WITH_MATH)
  add_library(ram_math SHARED ${SOURCES} ${HEADERS})
  if (RAM_WITH_CORE)
    target_link_libraries(ram_math ram_core)
  endif (RAM_WITH_CORE)
  
  set_target_properties(ram_math PROPERTIES
    DEFINE_SYMBOL RAM_PKG_MATH
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )
  
  test_module(math "ram_math")
endif (RAM_WITH_MATH)
