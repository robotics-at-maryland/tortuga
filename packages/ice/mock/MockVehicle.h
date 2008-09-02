#ifndef _RAM_MOCK_MOCKVEHICLE_H
#define _RAM_MOCK_MOCKVEHICLE_H

#include <ram_ice.h>
#include "MockIMU.h"
#include "MockPowerSource.h"
#include "MockDepthSensor.h"

namespace ram {
    namespace mock {
        class MockVehicle : virtual public vehicle::IVehicle
        {
        private:
            vehicle::DeviceList devices;
            vehicle::IMUPrx getFirstIMU()
            {
                for (vehicle::DeviceList::iterator iter = devices.begin();
                    iter != devices.end() ; iter++)
                    if ((*iter)->ice_isA("::ram::vehicle::IMU"))
                        return vehicle::IMUPrx::uncheckedCast(*iter);
				// If no device was found, cry about it
				throw Ice::ObjectNotFoundException();
            }
            vehicle::DepthSensorPrx getFirstDepthSensor()
            {
                for (vehicle::DeviceList::iterator iter = devices.begin();
                    iter != devices.end() ; iter++)
                    if ((*iter)->ice_isA("::ram::vehicle::DepthSensor"))
                        return vehicle::DepthSensorPrx::uncheckedCast(*iter);
				// If no device was found, cry about it
				throw Ice::ObjectNotFoundException();
            }
            
            void addDevice(vehicle::DevicePrx prx)
            { devices.push_back(prx); }
        public:
            MockVehicle(const ::Ice::Current&c)
            {
                addDevice(vehicle::DevicePrx::uncheckedCast(c.adapter->addWithUUID(new MockIMU("Yoyodyne Propulsion Systems IMU"))));
                addDevice(vehicle::DevicePrx::uncheckedCast(c.adapter->addWithUUID(new MockDepthSensor("Absolute Pressure Sensor"))));
                addDevice(vehicle::DevicePrx::uncheckedCast(c.adapter->addWithUUID(new MockPowerSource("Battery 1"))));
                addDevice(vehicle::DevicePrx::uncheckedCast(c.adapter->addWithUUID(new MockDepthSensor("Bottom Ranging Sonar"))));
                addDevice(vehicle::DevicePrx::uncheckedCast(c.adapter->addWithUUID(new MockPowerSource("Acme Thermonuclear Reactor"))));
                addDevice(vehicle::DevicePrx::uncheckedCast(c.adapter->addWithUUID(new MockDepthSensor("Surface Ranging Sonar"))));
            }
            
            inline virtual vehicle::DeviceList getDevices(const ::Ice::Current&c)
            { return devices; }
            
            inline virtual vehicle::DevicePrx getDeviceByName(const std::string& name, const ::Ice::Current&c)
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
            { /* do nothing */ }
            
            inline virtual void safeThrusters(const ::Ice::Current&c)
            { std::cout << "Thrusters safed!" << std::endl; }
            
            inline virtual void unsafeThrusters(const ::Ice::Current&c)
            { std::cout << "Thrusters unsafed!" << std::endl; }
            
            inline virtual void dropMarker(const ::Ice::Current&c)
            { std::cout << "Marker dropped!" << std::endl; }
        };
    }
}

#endif
