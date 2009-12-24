/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/include/device/IIMU.h
 */

#ifndef RAM_VEHICLE_DEVICE_IIMU_06_25_2007
#define RAM_VEHICLE_DEVICE_IIMU_06_25_2007

// STD Includes
#include <string>

// Project Includes
#include "vehicle/include/device/IStateEstimatorDevice.h"
#include "math/include/Quaternion.h"
#include "math/include/Vector3.h"

// Must Be Included last
#include "vehicle/include/Export.h"


// Forward declare structure from imuapi.h
struct _RawIMUData;
typedef _RawIMUData RawIMUData;

namespace ram {
namespace vehicle {
namespace device {

typedef struct _IMUPacket {
    // Contains extraneous information for the IMU data
    int imuNum;
    double timestep;

    /** Rotates data from the IMU to the Vehicle frame */
    double IMUToVehicleFrame[3][3];

    /** Bias based on vehicles local mag field */
    double magXBias;
    double magYBias;
    double magZBias;

    /** Biased based on flaws in the gyro */
    double gyroXBias;
    double gyroYBias;
    double gyroZBias;

    /** Magnetic Corruption Threshold based on experimental vehicle data */
    double magCorruptThresh;

    /** Nominal value of magnetic vector length obtained experimentally */
    double magNominalLength;

    IIMUPtr device;
    RawIMUData* rawData;
} IMUPacket;

/** Abstract interface for Intertial Measurement Units */    
class RAM_EXPORT IIMU : public IStateEstimatorDevice   // For getName
             // boost::noncopyable
{
public:
    static const core::Event::EventType UPDATE;
    
    virtual ~IIMU();
    
protected:
    IIMU(core::EventHubPtr eventHub = core::EventHubPtr(),
         std::string name = "UNNAMED");
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IIMU_06_25_2007
