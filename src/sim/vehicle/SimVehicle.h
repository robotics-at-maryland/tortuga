#ifndef _RAM_SIM_SIMVEHICLE_H
#define _RAM_SIM_SIMVEHICLE_H

#include <ram.h>
#include <Ice/Ice.h>
#include "SimDevice.h"
#include "../SimWorld.h"

namespace ram {
    namespace sim {
        class SimVehicle : virtual public vehicle::IVehicle, public BuoyantBody {
        private:
            btBoxShape collisionShape;
            vehicle::DeviceList devices;
            std::vector<SimDevice*> deviceServants;
            
            vehicle::IMUPrx getFirstIMU() {
                for (vehicle::DeviceList::iterator iter = devices.begin();
                    iter != devices.end() ; iter++)
                    if ((*iter)->ice_isA("::ram::vehicle::IMU"))
                        return vehicle::IMUPrx::uncheckedCast(*iter);
            }
            
            vehicle::DepthSensorPrx getFirstDepthSensor() {
                for (vehicle::DeviceList::iterator iter = devices.begin();
                    iter != devices.end() ; iter++)
                    if ((*iter)->ice_isA("::ram::vehicle::DepthSensor"))
                        return vehicle::DepthSensorPrx::uncheckedCast(*iter);
            }
            
            void addDevice(SimDevice* dev, const ::Ice::Current&c)
            {
                devices.push_back(vehicle::DevicePrx::uncheckedCast(c.adapter->addWithUUID(dev)));
                deviceServants.push_back(dev);
            }
        public:
            SimVehicle(const ::Ice::Current&c);
            
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
            { /* TODO */ }
            
            inline virtual void safeThrusters(const ::Ice::Current&c)
            { /* TODO */ }
            
            inline virtual void unsafeThrusters(const ::Ice::Current&c)
            { /* TODO */ }
            
            inline virtual void dropMarker(const ::Ice::Current&c)
            { /* TODO */ }
            
        protected:
            virtual void stepSimulation(SimWorld& world, btScalar timeStep)
            {
                BuoyantBody::stepSimulation(world, timeStep);
                for (std::vector<SimDevice*>::iterator iter = deviceServants.begin();
                     iter != deviceServants.end() ; iter++)
                    (*iter)->stepSimulation(world, timeStep);
            }
            
        };
    }
}

#endif
