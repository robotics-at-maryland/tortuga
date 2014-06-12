file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

set(LINK_LIBS
  ram_core
  ram_pattern
  ram_math
  )

if (RAM_WITH_DRIVERS)
  list(APPEND LINK_LIBS
    ram_bfin_spartan
    ram_carnetix
    ram_dvl
    ram_imu
    ram_sensor
    ram_thruster
    )
endif (RAM_WITH_DRIVERS)

if (RAM_WITH_VISION)
  list(APPEND LINK_LIBS "ram_vision")
else (RAM_WITH_VISION)
  list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/device/VisionVelocitySensor.cpp")
endif (RAM_WITH_VISION)

if (RAM_WITH_VEHICLE)  
  add_library(ram_vehicle SHARED ${SOURCES} ${HEADERS})
  target_link_libraries(ram_vehicle ${LINK_LIBS})
  set_target_properties(ram_vehicle PROPERTIES
    DEFINE_SYMBOL RAM_PKG_VEHICLE
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )

  set(TEST_VEHICLE_EXCLUDE_LIST)
  if (NOT RAM_WITH_VISION)
    set(VEHICLE_EXCLUDE_LIST "test/src/TestVisionVelocitySensor.cxx")
  endif (NOT RAM_WITH_VISION)
  test_module(vehicle "ram_vehicle")
endif (RAM_WITH_VEHICLE)
