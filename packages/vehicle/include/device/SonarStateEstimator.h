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
#include "vehicle/include/device/Common.h"
#include "vehicle/include/device/IStateEstimator.h"
#include "vehicle/include/device/Device.h"

#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"

#include "math/include/VectorN.h"
#include "math/include/MatrixN.h"

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

    virtual void orientationUpdate(math::Quaternion orientation,
                                   deviceType device,
				   double timeStamp = -1);

    virtual void velocityUpdate(math::Vector2 velocity,
                                deviceType device,
				double timeStamp = -1);

    virtual void positionUpdate(math::Vector2 position,
                                deviceType device,
				double timeStamp = -1);
    
    virtual void depthUpdate(double depth,
                             deviceType device,
			     double timeStamp = -1);
    
    virtual math::Quaternion getOrientation(std::string obj = "vehicle");

    virtual math::Vector2 getVelocity(std::string obj = "vehicle");

    virtual math::Vector2 getPosition(std::string obj = "vehicle");
    
    math::Vector2 getLeftPingerEstimatedPosition();

    math::Vector2 getRightPingerEstimatedPosition();

    virtual double getDepth(std::string obj = "vehicle");

    virtual bool hasObject(std::string obj);

    /** Create a linearized measurement model based on state estimator */
    static void createMeasurementModel(const math::VectorN& xHat, 
                                       math::MatrixN& result);

    /** Create a discrete time model of the system dynamics and noise
        propagation matrix based on the change in time between samples */
    static void discretizeModel(double dragDensity, double rvMag, double ts,
                                math::MatrixN& Ak, math::MatrixN& Rv);

    /** Computes the angle from the north inertial unit vector to the pinger*/
    static math::Radian findAbsPingerAngle(math::Quaternion vehicleOrientation,
                                           math::Vector3 relativePingerVector);

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
    /** Returns the change in time between estimator updates */
    virtual double getDeltaT();
    
private:
    /** Recieves updates from the Sonar system to update estimated */
    void onSonarEvent(core::EventPtr event);

    /** Called to update the state esimated filter based on a new pinger angle
     *
     *  @note The mutex is already held during this call
     */
    void pingerLeftFilterUpdate(math::Degree angle, double dt);

    /** Called to update the state esimated filter based on a new pinger angle
     *
     *  @note The mutex is already held during this call
     */
    void pingerRightFilterUpdate(math::Degree angle, double dt);

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

    /** The connection to the sonar events */
    core::EventConnectionPtr m_sonarConnection;

    /** The last time we got an update */
    double m_lastUpdateTime;

    /** The "xHat" vector (2D robot pos/vel and pinger positions) */
    math::VectorN m_stateHat;

    /** The state covariance matrix */
    math::MatrixN m_Rv;

    /** linear time invariant dynamical model for sub for in-plane translation 
        dxdt = A*x
        dxdt = A*x+B*u  (future work)
    **/
    math::MatrixN m_A;
    
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_SONARSTATEESTIMATOR_06_26_2009
