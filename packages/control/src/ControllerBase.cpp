/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/src/ControllerBase.cpp
 */

// STD Includes
#include <cmath>

#ifdef RAM_WINDOWS
#define M_PI 3.14159265358979323846
#endif


// Project Includes
#include "control/include/ControllerBase.h"
#include "control/include/ControlFunctions.h"

#include "vehicle/include/IVehicle.h"

#include "core/include/SubsystemMaker.h"
#include "core/include/EventHub.h"
#include "core/include/TimeVal.h"

#include "math/include/Helpers.h"
#include "math/include/Events.h"
//#include "imu/include/imuapi.h"


using namespace std;

namespace ram {
namespace control {

ControllerBase::ControllerBase(vehicle::IVehiclePtr vehicle,
                               core::ConfigNode config) :
    IController(config["name"].asString()),
    m_atDepth(false),
    m_atOrientation(false),
    m_depthThreshold(0),
    m_orientationThreshold(0),
    m_vehicle(vehicle)
{   
    init(config); 
}

ControllerBase::ControllerBase(core::ConfigNode config,
                               core::SubsystemList deps) :
    IController(config["name"].asString(),
                core::Subsystem::getSubsystemOfType<core::EventHub>(deps)),
    m_atDepth(false),
    m_atOrientation(false),
    m_depthThreshold(0),
    m_orientationThreshold(0),
    m_vehicle(core::Subsystem::getSubsystemOfType<vehicle::IVehicle>(deps))
{
    init(config);

//    if (config["holdCurrentHeading"].asInt(0))
//        holdCurrentHeading();
}

ControllerBase::~ControllerBase()
{
    unbackground(true);
}

math::Quaternion ControllerBase::yawVehicleHelper(
    const math::Quaternion& currentOrientation, double degrees)
{
    //use Helpers.cpp
    using namespace ram::math;

    //create rotation quaternion based on user input
    double rotationQuaternion[4];
    double rollAxis[3] = {0,0,1};
    quaternionFromEulerAxis(rollAxis,degrees*M_PI/180,rotationQuaternion);
  
    //rotate the quaternion and store in a new place
    math::Quaternion newQuaternion;
    quaternionCrossProduct(currentOrientation.ptr(), rotationQuaternion,
                           (double*)newQuaternion.ptr());

    return newQuaternion;
}

math::Quaternion ControllerBase::pitchVehicleHelper(
    const math::Quaternion& currentOrientation, double degrees)
{
    //use Helpers.cpp
    using namespace ram::math;

    //create rotation quaternion based on user input
    double rotationQuaternion[4];
    double rollAxis[3] = {0,1,0};
    quaternionFromEulerAxis(rollAxis,degrees*M_PI/180,rotationQuaternion);
  
    //rotate the quaternion and store in a new place
    math::Quaternion newQuaternion;
    quaternionCrossProduct(currentOrientation.ptr(), rotationQuaternion,
                           (double*)newQuaternion.ptr());

    return newQuaternion;
    
}

math::Quaternion ControllerBase::rollVehicleHelper(
    const math::Quaternion& currentOrientation, double degrees)
{
    //use Helpers.cpp
    using namespace ram::math;

    //create rotation quaternion based on user input
    double rotationQuaternion[4];
    double rollAxis[3] = {1,0,0};
    quaternionFromEulerAxis(rollAxis,degrees*M_PI/180,rotationQuaternion);
  
    //rotate the quaternion and store in a new place
    math::Quaternion newQuaternion;
    quaternionCrossProduct(currentOrientation.ptr(), rotationQuaternion,
                           (double*)newQuaternion.ptr());

    return newQuaternion;

}

math::Quaternion ControllerBase::holdCurrentHeadingHelper(
    const math::Quaternion& currentOrientation)
{
    //find approximation of vehicle yaw based off orientation
    math::Radian yaw = currentOrientation.getYaw();

    //compute "level" quaternion based off yaw value
    math::Quaternion q(math::Radian(yaw),math::Vector3::UNIT_Z);

    return q;
}

void ControllerBase::update(double timestep)
{
    // Get vehicle state
    math::Vector3 linearAcceleration(m_vehicle->getLinearAcceleration());
    math::Quaternion orientation(m_vehicle->getOrientation());
    math::Vector3 angularRate(m_vehicle->getAngularRate());
    double depth = m_vehicle->getDepth();
    
    // Run the base values
    math::Vector3 translationalForce;
    math::Vector3 rotationalTorque;

    // Call the base class update function
    doUpdate(timestep, linearAcceleration, orientation, angularRate, depth,
             translationalForce, rotationalTorque);

    // Actually set motor values
    m_vehicle->applyForcesAndTorques(translationalForce, rotationalTorque);
    
    // We use to be at depth now we aren't
    if (m_atDepth && !atDepth())
    {
        m_atDepth = false;
    }
    // We weren't at depth, now we are
    else if (!m_atDepth && atDepth())
    {
        publishAtDepth(getDepth());
    }

    // We used to be at orientation now we aren't
    if (m_atOrientation && !atOrientation())
    {
        m_atOrientation = false;
    }
    // We weren't at depth, now we are
    else if (!m_atOrientation && atOrientation())
    {
        publishAtOrientation(getDesiredOrientation());
    }
}

void ControllerBase::newDepthSet(const double& newDepth)
{
    // Publish event indicating new update
    math::NumericEventPtr event(new math::NumericEvent());
    event->number = newDepth;
    publish(IController::DESIRED_DEPTH_UPDATE, event);

    // Make sure to publish if we set a depth that is within our range
    if (atDepth())
        publishAtDepth(newDepth);
}

void ControllerBase::newDesiredOrientationSet(
    const math::Quaternion& newOrientation)
{
    math::OrientationEventPtr event(new math::OrientationEvent());
    event->orientation = newOrientation;
    publish(IController::DESIRED_ORIENTATION_UPDATE, event);
    
    
    if(atOrientation())
        publishAtOrientation(newOrientation);
}

void ControllerBase::init(core::ConfigNode config)
{
    // Load threshold for being at depth
    m_depthThreshold = config["depthThreshold"].asDouble(DEPTH_TOLERANCE);
    m_orientationThreshold =
        config["orientationThreshold"].asDouble(ORIENTATION_THRESHOLD);
}

void ControllerBase::publishAtDepth(const double& depth)
{
    m_atDepth = true;
    math::NumericEventPtr event(new math::NumericEvent());
    event->number = depth;
    publish(IController::AT_DEPTH, event);
}

void ControllerBase::publishAtOrientation(const math::Quaternion& orientation)
{
    m_atOrientation = true;
    math::OrientationEventPtr event(new math::OrientationEvent());
    event->orientation = orientation;
    publish(IController::AT_ORIENTATION, event);        
}
        
} // namespace control
} // namespace ram
