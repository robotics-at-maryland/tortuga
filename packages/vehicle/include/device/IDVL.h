/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vehicle/include/device/IDVL.h
 */

#ifndef RAM_VEHICLE_DEVICE_IDVL_11_20_2009
#define RAM_VEHICLE_DEVICE_IDVL_11_20_2009

// STD Includesb
#include <string>

// Project Includes
#include "vehicle/include/device/IStateEstimatorDevice.h"

#include "math/include/Vector2.h"
#include "math/include/Vector3.h"

// Must Be Included last
#include "vehicle/include/Export.h"


// Forward declare structure from dvlapi.h
struct _RawDVLData;
typedef _RawDVLData RawDVLData;

namespace ram {
namespace vehicle {
namespace device {

typedef struct _DVLPacket {
    // Contains extraneous information for the DVL data
    int dvlNum;
    double timestep;

    IDVLPtr device;
    RawDVLData* rawData;
} DVLPacket;
    
/** Represents a sensor that returns the velocity in feet */
class RAM_EXPORT IDVL : public IStateEstimatorDevice // For getName
             // boost::noncopyable
{
public:
    static const core::Event::EventType UPDATE;
    
    virtual ~IDVL();

    /** The location of the velocity sensor on the vehicle */
    virtual math::Vector3 getLocation() = 0;
    
protected:
    IDVL(core::EventHubPtr eventHub = core::EventHubPtr(),
                    std::string name = "UNNAMED");
};

} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IDVL_11_20_2009
