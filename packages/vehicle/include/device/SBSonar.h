/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/SBSonar.h
 */

#ifndef RAM_VEHICLE_DEVICE_SBPOWER_07_19_2008
#define RAM_VEHICLE_DEVICE_SBPOWER_07_19_2008

// STD Includes
#include <string>

// Project Includes
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/ISonar.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {
    
class RAM_EXPORT SBSonar :
        public Device,
        public ISonar
             // boost::noncopyable
{
public:
    /** Generated when we get a new ping */
    static const core::Event::EventType UPDATE;

    SBSonar(core::ConfigNode config,
            core::EventHubPtr eventHub = core::EventHubPtr(),
            IVehiclePtr vehicle = IVehiclePtr());
    
    virtual ~SBSonar();

    virtual math::Vector3 getDirection();

    virtual double getRange();
    
    virtual core::TimeVal pingTime();

    // Not used updatable stuff
    
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
    
private:
    /** SensorBoard::TEMPSENSOR_UPDATE event handler */
    void onSonarUpdate(core::EventPtr event);
    
    core::ReadWriteMutex m_mutex;
    math::Vector3 m_direction;
    double m_range;
    core::TimeVal m_pingTime;

    /** Sensor Board from which to access the hardware */
    SensorBoardPtr m_sensorBoard;

    /** SONAR_UPDATE event connection */
    core::EventConnectionPtr m_connection;
};

} // namespace device
} // namespace vehicle
} // namespace ram
    
#endif // RAM_VEHICLE_DEVICE_SBPOWER_07_19_2008
