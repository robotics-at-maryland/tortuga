
if (RAM_WITH_PLOT)
  #find_package(wxWidgets REQUIRED COMPONENTS std aui)

  include( ${wxWidgets_USE_FILE} )
  include_directories("include")

  set(PLOT_LIBS
    ram_core
    ram_network
    ram_control
    ram_estimation
    ram_vehicle
    ${wxWidgets_LIBRARIES}
    )

  file(GLOB SOURCES "src/*.cpp")
  file(GLOB HEADERS "include/*.h")

  add_executable(ramplot ${SOURCES} ${HEADERS})
  target_link_libraries(ramplot ${PLOT_LIBS})
endif (RAM_WITH_PLOT)
