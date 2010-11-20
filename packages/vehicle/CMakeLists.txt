file(GLOB HEADERS "include/*.h")
file(GLOB SOURCES "src/*.cpp")

set(LINK_LIBS
  ram_core
  ram_pattern
  ram_math
  )

file(GLOB DEVICE_HEADERS "include/device/*.h")
file(GLOB DEVICE_SOURCES "src/device/*.cpp")
if (NOT RAM_WITH_VISION)
  list(REMOVE_ITEM DEVICE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/device/VisionVelocitySensor.cpp")
endif (NOT RAM_WITH_VISION)

set(DEVICE_LIBRARIES
  ram_bfin_spartan
  ram_carnetix
  ram_dvl
  ram_imu
  ram_sensor
  ram_thruster
  )

file(GLOB_RECURSE ESTIMATOR_HEADERS "include/estimator/*.h")
file(GLOB_RECURSE ESTIMATOR_SOURCES "src/estimator/*.cpp")

if (RAM_WITH_VEHICLE)
  add_library(ram_vehicle_device SHARED ${DEVICE_SOURCES} ${DEVICE_HEADERS})
  target_link_libraries(ram_vehicle_device ${DEVICE_LIBRARIES})
  set_target_properties(ram_vehicle_device PROPERTIES
    DEFINE_SYMBOL RAM_PKG_VEHICLE
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )

  add_library(ram_vehicle_estimator SHARED ${ESTIMATOR_SOURCES} ${ESTIMATOR_HEADERS})
  set_target_properties(ram_vehicle_estimator PROPERTIES
    DEFINE_SYMBOL RAM_PKG_VEHICLE
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )
  
  add_library(ram_vehicle SHARED ${SOURCES} ${HEADERS})
  target_link_libraries(ram_vehicle
    ram_vehicle_device
    ram_vehicle_estimator
    ${LINK_LIBS}
    )
  set_target_properties(ram_vehicle PROPERTIES
    DEFINE_SYMBOL RAM_PKG_VEHICLE
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )

  set(TEST_VEHICLE_EXCLUDE_LIST)
  if (NOT RAM_WITH_VISION)
    set(TEST_VEHICLE_EXCLUDE_LIST "test/src/TestVisionVelocitySensor.cxx")
  endif (NOT RAM_WITH_VISION)
  test_module(vehicle "${TEST_VEHICLE_EXCLUDE_LIST}")
endif (RAM_WITH_VEHICLE)
