/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/IDepthSensor.h
 */

#ifndef RAM_VEHICLE_DEVICE_IDEPTHSENSOR_06_05_2008
#define RAM_VEHICLE_DEVICE_IDEPTHSENSOR_06_05_2008

// STD Includesb
#include <string>

// Project Includes
#include "vehicle/include/device/IStateEstimatorDevice.h"

#include "math/include/Vector3.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {

typedef struct _DepthPacket {
    // Extraneous information for depth data
    double depth;
    IDepthSensorPtr device;
} DepthPacket;

/** Represents a sensor that returns the depth in feet */
class RAM_EXPORT IDepthSensor : public IStateEstimatorDevice // For getName
             // boost::noncopyable
{
public:
    static const core::Event::EventType UPDATE;
    
    virtual ~IDepthSensor();

    /** The current depth the sensor is reading in feet */
    virtual double getDepth() = 0;

    /** The location of the depth sensor on the vehicle */
    virtual math::Vector3 getLocation() = 0;
    
protected:
    IDepthSensor(core::EventHubPtr eventHub = core::EventHubPtr(),
                 std::string name = "UNNAMED");
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IDEPTHSENSOR_06_05_2008
