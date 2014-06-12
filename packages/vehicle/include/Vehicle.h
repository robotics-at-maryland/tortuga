/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Vehicle.h
 */

#ifndef RAM_VEHICLE_VEHICLE_06_11_2007
#define RAM_VEHICLE_VEHICLE_06_11_2007

// STD Includes
#include <string>

// Library Includes
#include "boost/tuple/tuple.hpp"

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/EventPublisher.h"
#include "core/include/ReadWriteMutex.h"
#include "core/include/Updatable.h"

#include "vehicle/include/Common.h"
#include "vehicle/include/IVehicle.h"

#include "math/include/MatrixN.h"
#include "math/include/VectorN.h"
#include "math/include/Vector3.h"

namespace ram {
namespace vehicle {

typedef boost::tuple<math::Vector3, math::Vector3, math::Vector3,
                     math::Vector3, math::Vector3, math::Vector3> Tuple6Vector3;

class Vehicle : public IVehicle, public core::Updatable
{
public:
    Vehicle(core::ConfigNode config, 
            core::SubsystemList deps = core::SubsystemList());
        
    virtual ~Vehicle();
    
    /** Gets the device associated with the given name  <b>NOT THREAD SAFE</b>*/
    virtual device::IDevicePtr getDevice(std::string name);

    virtual std::vector<std::string> getDeviceNames();

    virtual void safeThrusters();

    virtual void unsafeThrusters(); 

    virtual void dropMarker();

    virtual void fireTorpedo();

    virtual void dropMarkerIndex(int index);

    virtual void fireTorpedoIndex(int index);

    virtual void releaseGrabber();

    virtual void closeGrabber();

    virtual void corruptIMU(bool c);

    
    virtual void applyForcesAndTorques(const math::Vector3& force,
                                       const math::Vector3& torque);

    /** This is <b>NOT</b> thread safe */
    /** return 0 indicates success, everything else indicates failure */
    virtual int _addDevice(device::IDevicePtr device);

    /** Sets the priority to all background devices threads */
    virtual void setPriority(core::IUpdatable::Priority priority);

    virtual core::IUpdatable::Priority getPriority() {
        return Updatable::getPriority();
    }

    /** Sets the affinity for all background devices threads */
    virtual void setAffinity(size_t affinity);

    virtual int getAffinity() {
        return Updatable::getAffinity();
    }
         
    /** Backgrounds all devices with the given update interval */
    virtual void background(int interval);

    /** Unbackgrounds all devices */
    virtual void unbackground(bool join = false);
    
    virtual bool backgrounded() {
        return Updatable::backgrounded();
    };

    /** Currently just manually grabs depth */
    virtual void update(double timestep);


    /** compute vector of forces to apply to thrusters at given offsets
       so that there is no net torque.  This assumes that the thrusters
       are applying a torque in opposite directions*/

    math::MatrixN createControlSignalToThrusterForcesMatrix(
        Tuple6Vector3 thrusterLocations);

    
    virtual void setExtraThruster(int speed);

protected:    
    /** Returns true if all IThrusterPtrs now contain valid thrusters */
    bool lookupThrusterDevices();
    
private:
    core::ConfigNode m_config;
    
    NameDeviceMap m_devices;

    std::string m_starboardThrusterName;
    vehicle::device::IThrusterPtr m_starboardThruster;
    std::string m_portThrusterName;
    vehicle::device::IThrusterPtr m_portThruster;
    std::string m_foreThrusterName;
    vehicle::device::IThrusterPtr m_foreThruster;
    std::string m_aftThrusterName;
    vehicle::device::IThrusterPtr m_aftThruster;
    std::string m_topThrusterName;
    vehicle::device::IThrusterPtr m_topThruster;
    std::string m_bottomThrusterName;
    vehicle::device::IThrusterPtr m_bottomThruster;
    std::string m_extraThrusterName;
    vehicle::device::IThrusterPtr m_extraThruster;
    math::Vector3 m_extraLocation;
    math::Vector3 m_extraDirection;
    double force;
    bool m_extraThrustOn;


    double m_topThrusterThrottle;

    std::string m_markerDropperName;
    vehicle::device::IPayloadSetPtr m_markerDropper;

    std::string m_torpedoLauncherName;
    vehicle::device::IPayloadSetPtr m_torpedoLauncher;

    std::string m_grabberName;
    vehicle::device::IPayloadSetPtr m_grabber;

    math::MatrixN m_controlSignalToThrusterForces;
    bool m_controlSignalToThrusterForcesCreated;
    
    enum thrusters {PORT = 0, STAR, TOP, FORE, BOT, AFT};
    enum forceAndThrustIndices {FX = 0, FY, FZ, TX, TY, TZ};
};
    
} // namespace vehicle
} // namespace ram
    
#endif // RAM_VEHICLE_VEHICLE_06_11_2007
