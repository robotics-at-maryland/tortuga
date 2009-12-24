/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/IStateEstimator.h
 */

#ifndef RAM_VEHICLE_DEVICE_ISTATEESTIMATOR_06_24_2009
#define RAM_VEHICLE_DEVICE_ISTATEESTIMATOR_06_24_2009

// STD Includes
#include <string>

// Project Includes
#include "vehicle/include/device/Common.h"
#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/device/IIMU.h"
#include "vehicle/include/device/IDVL.h"
#include "vehicle/include/device/IDepthSensor.h"
#include "math/include/Quaternion.h"
#include "math/include/Vector3.h"
#include "math/include/Vector2.h"

// Must Be Included last
#include "vehicle/include/Export.h"

// Forward declare structure from dvlapi.h
struct _RawDVLData;
typedef _RawDVLData RawDVLData;

namespace ram {
namespace vehicle {
namespace device {

// Forward declarations
struct _IMUPacket;
typedef _IMUPacket IMUPacket;

struct _DVLPacket;
typedef _DVLPacket DVLPacket;

struct _DepthPacket;
typedef _DepthPacket DepthPacket;

/** Abstract interface for a StateEstimator that fuses sensor data */
class RAM_EXPORT IStateEstimator : public IDevice         // For getName
             // boost::noncopyable
{
public:
    /** When the vehicles orientation changes (ram::math::OrientationEvent)*/
    static const core::Event::EventType ORIENTATION_UPDATE;
    
    /** When the vehicles linear accel. changes (ram::math::Vector3Event) */
    static const core::Event::EventType LINEAR_ACCEL_UPDATE;
    
    /** When the vehicles angular rate changes (ram::math::Vector3Event) */
    static const core::Event::EventType ANGULAR_RATE_UPDATE;
    
    /** When the vehicles depth changes (ram::math::NumericEvent) */
    static const core::Event::EventType DEPTH_UPDATE;

    /** When the vehicles position changes (ram::math::Vector2Event) */
    static const core::Event::EventType POSITION_UPDATE;

    /** When the vehicles velocity changes (ram::math::Vector2Event) */
    static const core::Event::EventType VELOCITY_UPDATE;
    
    virtual ~IStateEstimator();
    
    /** Update the estimator with new imu data */
    virtual void imuUpdate(IMUPacket* rawData) = 0;

    /** Update the estimator with new dvl data */
    virtual void dvlUpdate(DVLPacket* rawData) = 0;
    
    /** Update the estimator with a new depth */
    virtual void depthUpdate(DepthPacket* rawData) = 0;
    
    /** Get the latest estimated orientation */
    virtual math::Quaternion getOrientation(std::string obj = "vehicle") = 0;

    /** Get data relevant to the IMU */
    virtual math::Vector3 getLinearAcceleration() = 0;

    virtual math::Vector3 getMagnetometer() = 0;

    virtual math::Vector3 getAngularRate() = 0;

    /** Get the latest estimated velocity */
    virtual math::Vector2 getVelocity(std::string obj = "vehicle") = 0;

    /** Get the latest estimated position */
    virtual math::Vector2 getPosition(std::string obj = "vehicle") = 0;
    
    /** Get the latest estimated depth */
    virtual double getDepth(std::string obj = "vehicle") = 0;

    /** Get whether this state estimator contains this object */
    virtual bool hasObject(std::string obj) = 0;

protected:
    IStateEstimator(core::EventHubPtr eventHub = core::EventHubPtr(),
                    std::string name = "UNNAMED");

    void publishOrientation();

    void publishDepth();

    void publishVelocity();

    void publishPosition();
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_ISTATEESTIMATOR_06_24_2009
