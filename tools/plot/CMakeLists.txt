
if (RAM_WITH_PLOT)
  set(wxWidgets_ROOT_DIR /opt/ram/local)
  find_package(wxWidgets COMPONENTS core base REQUIRED)

  include( ${wxWidgets_USE_FILE} )

  set(PLOT_LIBS
    ram_core
    ram_network
    ${wxWidgets_LIBRARIES}
    )

  file(GLOB SOURCES "src/*.cpp")
  file(GLOB HEADERS "include/*.h")

  add_executable(ramplot ${SOURCES} ${HEADERS})
  target_link_libraries(ramplot ${PLOT_LIBS})
endif (RAM_WITH_PLOT)
