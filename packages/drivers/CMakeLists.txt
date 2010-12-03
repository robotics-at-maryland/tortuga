
if (UNIX AND RAM_WITH_DRIVERS)
  find_package(USB REQUIRED)
  include_directories("carnetix/include" ${USB_INCLUDE_DIRS})
  file(GLOB CARNETIX_HEADERS "carnetix/include/*.h")
  file(GLOB CARNETIX_SOURCES "carnetix/src/*.c")

  add_library(ram_carnetix SHARED
    ${CARNETIX_SOURCES} ${CARNETIX_HEADERS})
  target_link_libraries(ram_carnetix ${USB_LIBRARIES})
  set_target_properties(ram_carnetix PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )

  include_directories("dvl/include")
  file(GLOB DVL_HEADERS "dvl/include/*.h")
  file(GLOB DVL_SOURCES "dvl/src/*.c")

  add_library(ram_dvl SHARED
    ${DVL_SOURCES} ${DVL_HEADERS})
  set_target_properties(ram_dvl PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )

  include_directories("imu/include")
  file(GLOB IMU_HEADERS "imu/include/*.h")
  file(GLOB IMU_SOURCES "imu/src/*.c")

  add_library(ram_imu SHARED
    ${IMU_SOURCES} ${IMU_HEADERS})
  set_target_properties(ram_imu PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )

  include_directories("sensor-r5/include")
  file(GLOB SENSOR_HEADERS "sensor-r5/include/*.h")
  file(GLOB SENSOR_SOURCES "sensor-r5/src/*.c")

  add_library(ram_sensor SHARED
    ${SENSOR_SOURCES} ${SENSOR_HEADERS})
  set_target_properties(ram_sensor PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )

  include_directories("thruster/include")
  file(GLOB THRUSTER_HEADERS "thruster/include/*.h")
  file(GLOB THRUSTER_SOURCES "thruster/src/*.c")

  add_library(ram_thruster SHARED
    ${THRUSTER_SOURCES} ${THRUSTER_HEADERS})
  set_target_properties(ram_thruster PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )
endif ()

# bfin_spartan should be built with sonar feature
if (UNIX AND RAM_WITH_SONAR)
  remove_definitions(-Wno-deprecated)
  file(GLOB BFIN_SPARTAN_HEADERS "bfin_spartan/include/*.h")
  file(GLOB BFIN_SPARTAN_SOURCES "bfin_spartan/src/*.c")
  
  add_library(ram_bfin_spartan SHARED
    ${BFIN_SPARTAN_SOURCES} ${BFIN_SPARTAN_HEADERS})
  set_target_properties(ram_bfin_spartan PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LIBDIR}"
    )
endif ()
