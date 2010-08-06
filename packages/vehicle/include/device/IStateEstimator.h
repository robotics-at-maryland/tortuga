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
#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/device/Common.h"
#include "math/include/Quaternion.h"
#include "math/include/Vector2.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {

/**
 * Returned by the update function for which values get changed in
 * the estimator.
 *
 * Returned by updateX functions. Perform whatever operations need to
 * be done in the update function and then return which state values were
 * changed. To combine values, use '|'.
 *
 * Ex. To return that the velocity and position have both been changed:
 *     return State::POS | State::VEL;
 *
 * Return 0 if no states have been changed.
 */
struct StateFlag {
    static const int POS = 1;
    static const int VEL = 1 << 1;
    static const int DEPTH = 1 << 2;
    static const int ORIENTATION = 1 << 3;
};

/** Abstract interface for a StateEstimator that fuses sensor data */
class RAM_EXPORT IStateEstimator : public IDevice         // For getName
             // boost::noncopyable
{
public:
    virtual ~IStateEstimator();

    /**
     * Update function declarations. The timeStamp will be a
     * default of -1. The messages sent by the system will have
     * a timeStamp. Any changes made by the user should not have
     * a timeStamp. The implementations should act differently
     * based on whether they've been given a timeStamp
     */
    
    /** Update the estimator with a new orientation */
    virtual int orientationUpdate(math::Quaternion orientation,
				  double timeStamp) = 0;

    /** Update the estimator with a new velocity */
    virtual int velocityUpdate(math::Vector2 velocity,
			       double timeStamp) = 0;

    /** Update the estimator with a new position */
    virtual int positionUpdate(math::Vector2 position,
			       double timeStamp) = 0;
    
    /** Update the estimator with a new depth */
    virtual int depthUpdate(double depth,
			    double timeStamp) = 0;
    
    /** Get the latest estimated orientation */
    virtual math::Quaternion getOrientation(std::string obj = "vehicle") = 0;

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
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_ISTATEESTIMATOR_06_24_2009
