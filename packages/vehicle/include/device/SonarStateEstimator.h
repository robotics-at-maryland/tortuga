/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/SonarStateEstimator.h
 */

#ifndef RAM_VEHICLE_DEVICE_SONARSTATEESTIMATOR_06_26_2009
#define RAM_VEHICLE_DEVICE_SONARSTATEESTIMATOR_06_26_2009

// Project Includes
#include "vehicle/include/device/IStateEstimator.h"
#include "vehicle/include/device/Device.h"

#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {

/** Simple sonar back estimator, ie. it doesn't esimator anything */
class RAM_EXPORT SonarStateEstimator :
        public Device, // for getName
        public IStateEstimator
        // boost::noncopyable
{
public:
    SonarStateEstimator(core::ConfigNode config,
                        core::EventHubPtr eventHub = core::EventHubPtr(),
                        IVehiclePtr vehicle = IVehiclePtr());
    
    virtual ~SonarStateEstimator();

    virtual void orientationUpdate(math::Quaternion orientation);

    virtual void velocityUpdate(math::Vector2 velocity);

    virtual void positionUpdate(math::Vector2 position);
    
    virtual void depthUpdate(double depth);
    
    virtual math::Quaternion getOrientation();

    virtual math::Vector2 getVelocity();

    virtual math::Vector2 getPosition();
    
    virtual double getDepth();

    // Device Options
    virtual std::string getName() { return Device::getName(); }
    
    virtual void update(double timestep) {}

    virtual void setPriority(core::IUpdatable::Priority) {}

    virtual core::IUpdatable::Priority getPriority() {
        return IUpdatable::NORMAL_PRIORITY;
    }

    virtual void setAffinity(size_t) {};
    
    virtual int getAffinity() {
        return -1;
    };
    
    virtual void background(int interval) {
        //Updatable::background(interval);
    };
    
    virtual void unbackground(bool join = false) {
        //Updatable::unbackground(join);
    };
    virtual bool backgrounded() {
        return true;
        //return Updatable::backgrounded();
    };

    
protected:
    /** Recieves updates from the Sonar system to update estimated */
    void onSonarEvent(core::EventPtr event);

    /** Called to update the state esimated filter based on a new pinger angle
     *
     *  @note The mutex is already held during this call
     */
    void pinger0FilterUpdate(math::Degree angle);

    /** Called to update the state esimated filter based on a new pinger angle
     *
     *  @note The mutex is already held during this call
     */
    void pinger1FilterUpdate(math::Degree angle);

    /** Called to update the state esimated filter based on a new velocity
     *
     *  @note The mutex is already held during this call
     */
    void velocityFilterUpdate(math::Vector2 velocity);
    
    core::ReadWriteMutex m_mutex;

    /** The current estimated orientation */
    math::Quaternion m_estimatedOrientation;

    /** The current estimated velocity */
    math::Vector2 m_estimatedVelocity;
    
    /** The current estimated position */
    math::Vector2 m_estimatedPosition;

    /** The current esimated depth */
    double m_estimatedDepth;

    /** The current sensed orientation of the vehicle */
    math::Quaternion m_currentOrientation;

    /** The current sensed velocity of the vehicle */
    math::Vector2 m_currentVelocity;

    /** The current sensed depth */
    double m_currentDepth;
    
    /** The position of the pinger associated with ID 0 */
    math::Vector2 m_pinger0Position;

    /** The position of the pinger associated with ID 1 */
    math::Vector2 m_pinger1Position;

    /** The connection to the sonar events */
    core::EventConnectionPtr m_sonarConnection;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_SONARSTATEESTIMATOR_06_26_2009
