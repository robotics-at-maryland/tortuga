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

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"
#include "core/include/Updatable.h"
#include "core/include/AveragingFilter.h"

#include "vehicle/include/Common.h"
#include "vehicle/include/IVehicle.h"

namespace ram {
namespace vehicle {

class Vehicle : public IVehicle, public core::Updatable
{
public:
    Vehicle(core::ConfigNode config);
    
    virtual ~Vehicle() {}
    
    /** Gets the device associated with the given name  <b>NOT THREAD SAFE</b>*/
    virtual device::IDevicePtr getDevice(std::string name);

    virtual double getDepth();

    /** Truns <b>ON</b> the thruster safety */
    virtual void safeThruster();

    /** Turns <b>OFF</b> the thruster safety */
    virtual void unsafeThrusters(); 

    /** Drops one of the two markers */
    virtual void dropMarker();

    /** Returns 1 if the start switch is down 0 if its up */
    virtual int startStatus();

    /** Prints a line to the vehicle LCD screen */
    virtual void printLine(int line, std::string text);
    
    /** This is <b>NOT</b> thread safe */
    virtual void _addDevice(device::IDevicePtr device);

    virtual void background(int interval) {
        Updatable::background(interval);
    };
    
    virtual void unbackground(bool join = false) {
        Updatable::unbackground(join);
    };
    virtual bool backgrounded() {
        return Updatable::backgrounded();
    };

    /** Currently just manually grabs depth */
    virtual void update(double timestep);

    /** Records the next 5 depths readings and sets it as the offset */
    void calibrateDepth();
    
protected:
    struct VehicleState
    {
        double depth;
        bool startSwitch;
    };
    
    void getState(Vehicle::VehicleState& state);
    void setState(const Vehicle::VehicleState& state);

    
private:
    
    core::ConfigNode m_config;

    /** syncs access to the sensor api */
    boost::mutex m_sensorBoardMutex;
    int m_sensorFD;
    int m_markerNum;
    
    double m_depthCalibSlope;
    double m_depthCalibIntercept;
    
    core::ReadWriteMutex m_state_mutex;
    VehicleState m_state;
    
    NameDeviceMap m_devices;

    bool m_calibratedDepth;
    core::AveragingFilter<double, 5> m_depthFilter;
    double m_depthOffset;
};
    
} // namespace vehicle
} // namespace ram
    
#endif // RAM_VEHICLE_VEHICLE_06_11_2007
