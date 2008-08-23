#ifndef _RAM_SIM_SIMVEHICLE_H
#define _RAM_SIM_SIMVEHICLE_H

#include <ram.h>
#include <Ice/Ice.h>
#include "SimIMU.h"
#include "SimPowerSource.h"
#include "SimDepthSensor.h"
#include "../SimWorld.h"

namespace ram {
    namespace sim {
        class SimVehicle : virtual public vehicle::IVehicle, public BuoyantBody {
        private:
            btBoxShape collisionShape;
            vehicle::DeviceList devices;
            
            vehicle::IIMUPrx getFirstIMU() {
                for (vehicle::DeviceList::iterator iter = devices.begin();
                    iter != devices.end() ; iter++)
                    if ((*iter)->ice_isA("::ram::vehicle::IIMU"))
                        return vehicle::IIMUPrx::uncheckedCast(*iter);
            }
            
            vehicle::IDepthSensorPrx getFirstDepthSensor() {
                for (vehicle::DeviceList::iterator iter = devices.begin();
                    iter != devices.end() ; iter++)
                    if ((*iter)->ice_isA("::ram::vehicle::IDepthSensor"))
                        return vehicle::IDepthSensorPrx::uncheckedCast(*iter);
            }
            
            void addDevice(vehicle::IDevicePrx prx)
            { devices.push_back(prx); }
        public:
            SimVehicle(const ::Ice::Current&c)
            : collisionShape(btVector3(0.25,0.25,0.75)), BuoyantBody(80, &collisionShape, btVector3(5, 5, 10))
            {
                setBuoyantVolume(0.1);
                setCenterOfBuoyancyPosition(btVector3(0.05,0,0.005));
                addDevice(vehicle::IDevicePrx::uncheckedCast(c.adapter->addWithUUID(new SimIMU("Yoyodyne Propulsion Systems IMU"))));
                addDevice(vehicle::IDevicePrx::uncheckedCast(c.adapter->addWithUUID(new SimDepthSensor("Absolute Pressure Sensor"))));
                addDevice(vehicle::IDevicePrx::uncheckedCast(c.adapter->addWithUUID(new SimPowerSource("Battery 1"))));
                addDevice(vehicle::IDevicePrx::uncheckedCast(c.adapter->addWithUUID(new SimDepthSensor("Bottom Ranging Sonar"))));
                addDevice(vehicle::IDevicePrx::uncheckedCast(c.adapter->addWithUUID(new SimPowerSource("Acme Thermonuclear Reactor"))));
                addDevice(vehicle::IDevicePrx::uncheckedCast(c.adapter->addWithUUID(new SimDepthSensor("Surface Ranging Sonar"))));
            }
            
            inline virtual vehicle::DeviceList getDevices(const ::Ice::Current&c)
            { return devices; }
            
            inline virtual vehicle::IDevicePrx getDeviceByName(const std::string& name, const ::Ice::Current&c)
            {
                for (vehicle::DeviceList::iterator iter = devices.begin();
                    iter != devices.end() ; iter++)
                    if ((*iter)->getName() == name)
                        return *iter;
                // Throw an exception if no matching device was found.
                vehicle::DeviceNotFoundException e;
                e.reason = "Could not find a device named '";
                e.reason += name;
                e.reason += "'.";
                throw e;
            }
            
            inline virtual double getDepth(const ::Ice::Current&c)
            { return getFirstDepthSensor()->getDepth(); }
            
            inline virtual transport::Vector3Ptr getLinearAcceleration(const ::Ice::Current&c)
            { return getFirstIMU()->getLinearAcceleration(); }
            
            inline virtual transport::Vector3Ptr getAngularRate(const ::Ice::Current&c)
            { return getFirstIMU()->getAngularRate(); }
            
            inline virtual transport::QuaternionPtr getOrientation(const ::Ice::Current&c)
            { return getFirstIMU()->getOrientation(); }
            
            inline virtual void applyForcesAndTorques(const ::Ice::Current&c)
            { /* TODO */ }
            
            inline virtual void safeThrusters(const ::Ice::Current&c)
            { /* TODO */ }
            
            inline virtual void unsafeThrusters(const ::Ice::Current&c)
            { /* TODO */ }
            
            inline virtual void dropMarker(const ::Ice::Current&c)
            { /* TODO */ }
        };
    }
}

#endif
