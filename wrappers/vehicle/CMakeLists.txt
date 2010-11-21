
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
generate_wrappers( vehicle_device )
python_files( vehicle )
