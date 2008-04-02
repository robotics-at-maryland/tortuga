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
    Vehicle(core::ConfigNode config, 
            core::SubsystemList deps = core::SubsystemList());
        
    virtual ~Vehicle();
    
    /** Gets the device associated with the given name  <b>NOT THREAD SAFE</b>*/
    virtual device::IDevicePtr getDevice(std::string name);

    /** @copydoc IVehilce::getDeviceNames() */
    virtual std::vector<std::string> getDeviceNames();
    
    /** @copydoc IVehicle::getDepth() */
    virtual double getDepth();

    /** @copydoc IVehicle::getTemperatureNames() */
    virtual std::vector<std::string> getTemperatureNames();

    /** @copydoc IVehicle::getTemperatures() */
    virtual std::vector<int> getTemperatures();

    math::Vector3 getLinearAcceleration();

    math::Vector3 getAngularRate();
    
    math::Quaternion getOrientation();
    
    /** @copydoc IVehicle::safeThrusters() */
    virtual void safeThrusters();

    /** @copydoc IVehicle::unsafeThrusters */
    virtual void unsafeThrusters(); 

    /** @copydoc IVehicle::dropMaker */
    virtual void dropMarker();

    /** @copydoc IVehicle::startStatus */
    virtual int startStatus();

    /** Prints a line to the vehicle LCD screen */
    virtual void printLine(int line, std::string text);

    /** @copydoc IVehicle::applyForcesAndTorques */
    virtual void applyForcesAndTorques(const math::Vector3& force,
                                       const math::Vector3& torque);

    /** This is <b>NOT</b> thread safe */
    virtual void _addDevice(device::IDevicePtr device);

    virtual void background(int interval);
    
    virtual void unbackground(bool join = false);
    
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
        std::vector<int> temperatures;
    };
    
    void getState(Vehicle::VehicleState& state);
    void setState(const Vehicle::VehicleState& state);

    /** Grabs the IMU from the current list of devices */
    device::IIMUPtr getIMU();
    
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

    /** Values needed for thruster combination */
    double m_rStarboard;
    double m_rPort;
    double m_rFore;
    double m_rAft;
    double m_rTop;
    double m_rBottom;

    std::string m_starboardThruster;
    std::string m_portThruster;
    std::string m_foreThruster;
    std::string m_aftThruster;
    std::string m_topThruster;
    std::string m_bottomThruster;

    std::string m_imuName;
    vehicle::device::IIMUPtr m_imu;
};
    
} // namespace vehicle
} // namespace ram
    
#endif // RAM_VEHICLE_VEHICLE_06_11_2007
