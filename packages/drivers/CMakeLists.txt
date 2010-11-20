
if (UNIX AND RAM_WITH_DRIVERS)
  remove_definitions(-Wno-deprecated)
  file(GLOB BFIN_SPARTAN_HEADERS "bfin_spartan/include/*.h")
  file(GLOB BFIN_SPARTAN_SOURCES "bfin_spartan/src/*.c")
  
  add_library(ram_bfin_spartan SHARED
    ${BFIN_SPARTAN_SOURCES} ${BFIN_SPARTAN_HEADERS})

  include_directories("carnetix/include")
  file(GLOB CARNETIX_HEADERS "carnetix/include/*.h")
  file(GLOB CARNETIX_SOURCES "carnetix/src/*.c")

  add_library(ram_carnetix SHARED
    ${CARNETIX_SOURCES} ${CARNETIX_HEADERS})

  include_directories("dvl/include")
  file(GLOB DVL_HEADERS "dvl/include/*.h")
  file(GLOB DVL_SOURCES "dvl/src/*.c")

  add_library(ram_dvl SHARED
    ${DVL_SOURCES} ${DVL_HEADERS})

  include_directories("imu/include")
  file(GLOB IMU_HEADERS "imu/include/*.h")
  file(GLOB IMU_SOURCES "imu/src/*.c")

  add_library(ram_imu SHARED
    ${IMU_SOURCES} ${IMU_HEADERS})
endif ()
