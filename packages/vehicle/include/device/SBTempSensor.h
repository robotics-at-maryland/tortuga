/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/SBTempSensor.h
 */

#ifndef RAM_VEHICLE_DEVICE_SBTEMPSENSOR_06_24_2008
#define RAM_VEHICLE_DEVICE_SBTEMPSENSOR_06_24_2008

// STD Includesb
#include <string>

// Project Includes
#include "vehicle/include/device/ITempSensor.h"

// Must Be Included last
#include "vehicle/include/Export.h"
#include "vehicle/include/Common.h"
#include "vehicle/include/device/Device.h"

#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"

namespace ram {
namespace vehicle {
namespace device {

/** Implements the ITempSensor with the SensorBoard class */
class RAM_EXPORT SBTempSensor :
        public Device, // for getName
        public ITempSensor
        // boost::noncopyable
{
public:
    SBTempSensor(core::ConfigNode config,
                  core::EventHubPtr eventHub = core::EventHubPtr(),
                  IVehiclePtr vehicle = IVehiclePtr());
    
    virtual ~SBTempSensor();
    
    virtual int getTemp();

    //virtual bool inUse() = 0;

    virtual std::string getName() { return Device::getName(); }
    
    virtual void update(double timestep) {}
    
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
    void onTempSensorUpdate(core::EventPtr event);
    
    /** ID used to identify self in power source update messages */
    int m_id;

    core::ReadWriteMutex m_mutex;
    int m_temp;

    /** Sensor Board from which to access the hardware */
    SensorBoardPtr m_sensorBoard;

    /** TEMPSENSOR_UPDATE event connection */
    core::EventConnectionPtr m_connection;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_SBTEMPSENSOR_06_24_2008
