/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/SensorBoard.h
 */

#ifndef RAM_VEHICLE_DEVICE_SENSORBOARD_06_04_2008
#define RAM_VEHICLE_DEVICE_SENSORBOARD_06_04_2008

// STD Includes
#include <string>

// Project Includes
#include "vehicle/include/Common.h"
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/IDepthSensor.h"

#include "core/include/Event.h"
#include "core/include/Updatable.h"
#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"
#include "core/include/AveragingFilter.h"

#include "drivers/sensor-r5/include/sensorapi.h"

namespace ram {
namespace vehicle {
namespace device {

/** This class encompasses all comunication with the physical sensor board
 *
 *  This allows the the vehicle code to remain as backend agnostic as possible
 *  by providing services to the other IDevice objects the vehicle current has.
 *  The vehicle is not supposed to talk with device directly.
 */
class SensorBoard : public Device, // for getName, boost::noncopyable
                    public IDepthSensor,
                    public core::Updatable
{
public:
    enum PowerID
    {
        BATTERY_1,
        BATTERY_2,
        BATTERY_3,
        BATTERY_4,
        SHORE
    };
    
    /**
     * @defgroup Events SensorBoard Events
     */
    /* @{ */
    static const core::Event::EventType POWERSOURCE_UPDATE;
    static const core::Event::EventType TEMPSENSOR_UPDATE;
    static const core::Event::EventType THRUSTER_UPDATE;
    static const core::Event::EventType SONAR_UPDATE;
    /* @{ */

    static int NUMBER_OF_MARKERS;
    static int NUMBER_OF_TORPEDOS;
    
    /** Creates a device with the given file descriptor */
    SensorBoard(int deviceFD,
                core::ConfigNode config,
                core::EventHubPtr eventHub = core::EventHubPtr());
    
    /** Create a SensorBoard from the config file with the given vehicle */
    SensorBoard(core::ConfigNode config,
                core::EventHubPtr eventHub = core::EventHubPtr(),
                IVehiclePtr vehicle = IVehiclePtr());
    
    virtual ~SensorBoard();

    // IDevice interface methods
    virtual std::string getName() { return Device::getName(); }

    // IUpdatable interface methods
    
    /** Does a single iteration of the communication with the sensor board
     *
     *  Each iteration sends a set of thruster commands, get a depth and 1/10
     *  of all the other telemetry the board provides. So after ten update
     *  calls a complete set of telemetry will be returned by the board.
     */
    virtual void update(double timestep);

    virtual void setPriority(core::IUpdatable::Priority priority) {
        Updatable::setPriority(priority);
    }

    virtual core::IUpdatable::Priority getPriority() {
        return Updatable::getPriority();
    }

    virtual void setAffinity(size_t affinity) {
        Updatable::setAffinity(affinity);
    }

    virtual int getAffinity() {
        return Updatable::getAffinity();
    }

    virtual void background(int interval) {
        Updatable::background(interval);
    };
    
    virtual void unbackground(bool join = false) {
        Updatable::unbackground(join);
    };
    virtual bool backgrounded() {
        return Updatable::backgrounded();
    };
    
    // IDepth interface methods

    virtual math::Vector3 getLocation();

    // The class methods

    /** Sets the commanded thruster value
     *
     *  @param address
     *      The number of the thruster (1-6)
     *  @param count
     *      The value to command the thruster (-1024 to 1023)
     */
    virtual void setThrusterValue(int address, int count);

    /** Returns true if the thruster will repond to thruster commands
     *
     *  @param address
     *      The number of the thruster (1-6)
     */
    virtual bool isThrusterEnabled(int address);

    /** Enables of disables a desired thruster
     *
     *  @param address
     *      The number of the thruster (1-6)
     *  @param state
     *      True to enable the thruster, false to disable
     */
    virtual void setThrusterEnable(int address, bool state);

    /** Drops a marker (works only NUMBER_MARKERS times)
     *
     *  @return
     *      The number of the marker dropped, -1 if all markers are used.
     */
    virtual int dropMarker();

    /** Fire a torpedo (works only NUMBER_TORPEDOS times)
     *
     *  @return
     *      The number of the torpedo fired, -1 if all torpedos are used.
     */
    virtual int fireTorpedo();

    /** Releases the object in the treasure grabber. Only works once.
     *
     *  @return
     *      0 if the grabber successfully released the object. -1 if
     *      the object was already released.
     */
    virtual int releaseGrabber();
    
   /** Returns true if the vehicle can draw power from the power source
     *
     *  @param address
     *      The number of the power source (0-4)
     */
    virtual bool isPowerSourceEnabled(int address);


    /** Returns true if the vehicle is drawing power from the power source
     *
     *  @param address
     *      The number of the power source (0-4)
     */
    virtual bool isPowerSourceInUse(int address);

    /** Enables or disables the ability to draw power from the given source
     *
     *  @param address
     *      The number of the power source (0-4)
     *  @param state
     *      True to enable the power source, false to disable
     */
    virtual void setPowerSouceEnabled(int address, bool state);

    /** Enables or disables the DVl power source
     *
     * @param state
     *      True to enable the power source, false to disable
     */
    virtual void setDVLPowerEnabled(bool state);

    /** This returns the voltage of the main bus
     *
     *   This does so by calculting the average voltage of all currently
     *   in use power sources.  Its currently needed because we don't monitor
     *   the main bus voltage directly.
     */
    virtual double getMainBusVoltage();

protected:
    // Makes easy access to the sensor board and allows testing
    virtual void setSpeeds(int s1, int s2, int s3, int s4, int s5, int s6);

    virtual int partialRead(struct boardInfo* telemetry);

    virtual int readDepth();

    virtual void setThrusterSafety(int state);

    virtual void setBatteryState(int state);
    
    virtual void dropMarker(int markerNum);

    virtual void setServoPosition(unsigned char servoNumber,
                                  unsigned short position);

    virtual void setServoEnable(unsigned char mask);

    virtual void setServoPower(unsigned char power);

    virtual void setDVLPower(unsigned char power);
    
    virtual void syncBoard();
    
private:
    struct VehicleState
    {
        double depth;
        int thrusterValues[6];
        struct boardInfo telemetry;
        double mainBusVoltage;
    };

    /** Opens the FD if needed and syncs with the board */
    void establishConnection();

    bool handleReturn(int ret);

    /** Triggers the depth event  */
    void depthEvent(double depth);
    
    /** Triggers power source events */
    void powerSourceEvents(struct boardInfo* telemetry);

    /** Triggers temperature sensor events */
    void tempSensorEvents(struct boardInfo* telemetry);

    /** Triggers motor current events */
    void thrusterEvents(struct boardInfo* telemetry);

    /** Triggers the sonar event */
    void sonarEvent(struct boardInfo* telemetry);
    
    /** Calculates the bus voltage for getMainBusVoltage */
    double calculateMainBusVoltage(struct boardInfo* telemetry);

    /** Mutex which protects access to state */
    core::ReadWriteMutex m_stateMutex;

    /** Vehicle state */
    VehicleState m_state;

    // Hacked depth calibration stuff
    double m_depthCalibSlope;
    double m_depthCalibIntercept;

    // Location of the depth sensor
    math::Vector3 m_location;

    //bool m_calibratedDepth;
    //core::AveragingFilter<double, 5> m_depthFilter;
    //double m_depthOffset;
    
    /** The device file to open the file descriptor from */
    std::string m_deviceFile;

    /** Protects access to the SB device */
    boost::mutex m_deviceMutex;
    
    /** The file descriptor which is connected to the SB's USB port */
    int m_deviceFD;

    /** The fire servo position for the first servo */
    int m_servo1FirePosition;

    /** The fire servo position for the second servo */
    int m_servo2FirePosition;

    /** The fire servo position for the treasure grabber */
    int m_servo3FirePosition;

    /** The fire servo position for the treasure grabber */
    int m_servo4FirePosition;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_SENSORBOARD_06_04_2008
