/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/include/MockStateEstimator.h
 */

#ifndef RAM_VEHICLE_MOCKSTATEESTIMATOR_06_26_2009
#define RAM_VEHICLE_MOCKSTATEESTIMATOR_06_26_2009

// STD Includes
#include <map>
#include <string>

// Project Includes
#include "vehicle/include/device/Common.h"
#include "vehicle/include/device/IStateEstimator.h"
#include "vehicle/include/device/Device.h"
#include "core/include/ConfigNode.h"

// Must Be Included last
//#include "vehicle/include/Export.h"


class MockStateEstimator : public ram::vehicle::device::IStateEstimator,
                           public ram::vehicle::device::Device
{
public:
    MockStateEstimator(std::string name) :
        IStateEstimator(ram::core::EventHubPtr()),
        Device(name) {}
    
    MockStateEstimator(ram::core::ConfigNode config,
               ram::core::EventHubPtr eventHub =
               ram::core::EventHubPtr(),
               ram::vehicle::IVehiclePtr vehicle_ =
               ram::vehicle::IVehiclePtr()) :
        IStateEstimator(eventHub),
        Device(config["name"].asString()) {}

    virtual void orientationUpdate(ram::math::Quaternion orientation_,
				   double timeStamp_)
        { updateOrientation = orientation_; timeStamp = timeStamp_; }
    
    virtual void velocityUpdate(ram::math::Vector2 velocity_,
                                double timeStamp_)
        { updateVelocity = velocity_; timeStamp = timeStamp_; }
    
    virtual void positionUpdate(ram::math::Vector2 position_,
				double timeStamp_)
        { updatePosition = position_; timeStamp = timeStamp_; }
    
    virtual void depthUpdate(double depth_,
                             double timeStamp_)
        { updateDepth = depth_; timeStamp = timeStamp_; }
    
    virtual ram::math::Quaternion getOrientation(
        std::string obj = "vehicle") { return orientation[obj]; }

    virtual ram::math::Vector2 getVelocity(std::string obj = "vehicle") {
	return velocity[obj]; }

    virtual ram::math::Vector2 getPosition(std::string obj = "vehicle") {
	return position[obj]; }
    
    virtual double getDepth(std::string obj = "vehicle") { return depth[obj]; }

    virtual bool hasObject(std::string obj) {
	return obj == "vehicle" || obj == "buoy"; }

    ram::math::Quaternion updateOrientation;
    ram::math::Vector2 updateVelocity;
    ram::math::Vector2 updatePosition;
    double updateDepth;
    double timeStamp;
    
    std::map<std::string, ram::math::Quaternion> orientation;
    std::map<std::string, ram::math::Vector2> velocity;
    std::map<std::string, ram::math::Vector2> position;
    std::map<std::string, double> depth;
    
    virtual std::string getName() {
        return ram::vehicle::device::Device::getName();
    }

    virtual void setPriority(ram::core::IUpdatable::Priority) {};
    virtual ram::core::IUpdatable::Priority getPriority() {
        return ram::core::IUpdatable::NORMAL_PRIORITY;
    };
    virtual void setAffinity(size_t) {};
    virtual int getAffinity() {
        return -1;
    };
    virtual void update(double) {}
    virtual void background(int) {}
    virtual void unbackground(bool) {}
    virtual bool backgrounded() { return false; }
};

#endif // RAM_VEHICLE_MOCKSTATEESTIMATOR_06_26_2009
