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

// Project Includes
#include "vehicle/include/device/IDevice.h"
#include "math/include/Quaternion.h"
#include "math/include/Vector2.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {

/** Abstract interface for a StateEstimator that fuses sensor data */
class RAM_EXPORT IStateEstimator : public IDevice         // For getName
             // boost::noncopyable
{
public:
    virtual ~IStateEstimator();

    /** Update the estimator with a new orientation */
    virtual void orientationUpdate(math::Quaternion orientation,
				   double timeStamp) = 0;

    /** Update the estimator with a new velocity */
    virtual void velocityUpdate(math::Vector2 velocity,
				double timeStamp) = 0;

    /** Update the estimator with a new position */
    virtual void positionUpdate(math::Vector2 position,
				double timeStamp) = 0;
    
    /** Update the estimator with a new depth */
    virtual void depthUpdate(double depth,
			     double timeStamp) = 0;
    
    /** Get the latest estimated orientation */
    virtual math::Quaternion getOrientation() = 0;

    /** Get the latest estimated velocity */
    virtual math::Vector2 getVelocity() = 0;

    /** Get the latest estimated position */
    virtual math::Vector2 getPosition() = 0;
    
    /** Get the latest estimated depth */
    virtual double getDepth() = 0;

    
protected:
    IStateEstimator(core::EventHubPtr eventHub = core::EventHubPtr(),
                    std::string name = "UNNAMED");
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_ISTATEESTIMATOR_06_24_2009
