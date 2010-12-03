
set(WRAPPED_HEADERS
  IVehicle.h
  Events.h
)
gccxml( vehicle "${WRAPPED_HEADERS}" )
generate_wrappers( vehicle )

set(DEVICE_WRAPPED_HEADERS
  device/Common.h
  device/IDevice.h
  device/IThruster.h
  device/IDepthSensor.h
  device/IPowerSource.h
  device/ITempSensor.h
  device/IPayloadSet.h
  device/IIMU.h
  device/ISonar.h
  device/IVelocitySensor.h
  device/IPositionSensor.h
)
make_directory( ${CMAKE_CURRENT_BINARY_DIR}/device )
gccxml( vehicle "${DEVICE_WRAPPED_HEADERS}" )
set(vehicle_device_WRAPPER_BASE "ram_vehicle")
set(vehicle_device_WRAPPER_DIRECTORY_EXT "device")
generate_wrappers( vehicle_device )
python_files( vehicle )

test_wrapper( vehicle "ram_vehicle" )

set(VEHICLE_DEVICE_WRAPPER_DIRECTORY "test/src/device")
test_wrapper( vehicle_device "ram_vehicle" )
