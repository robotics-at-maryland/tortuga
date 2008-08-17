#ifndef _RAM_MOCK_MOCKVEHICLE_H
#define _RAM_MOCK_MOCKVEHICLE_H

#include <ram.h>
#include <stdlib.h>
#include "MockUtil.h"
#include "MockIMU.h"

namespace ram {
    class MockVehicle : virtual public vehicle::IVehicle
    {
    private:
        vehicle::Dictionary devices;
        vehicle::IIMUPrx getFirstIMU()
        {
            for (DeviceDictionary::iterator iter = devices.begin();
                iter != devices.end();
                iter++)
            {
                if (iter.second.ice_isA("::ram::vehicle::IIMU"))
                    return vehicle::IIMU::uncheckedCast(iter.second());
            }
        }
    public:
        MockVehicle()
        {
            vehicle::IIMU
            MockIMU* imu = new MockIMU;
            
            devices["Yoyodyne IMU"] = /*  TODO  */;
        }
        
        inline virtual vehicle::DeviceDictionary getDevices(const ::Ice::Current&c)
        {
            vehicle::DeviceDictionary dict;
            return dict;
        }
        
        inline virtual double getDepth(const ::Ice::Current&c)
        { return randUpTo(20); }
        
        inline virtual math::Vector3Ptr getLinearAcceleration(const ::Ice::Current&c)
        { return getFirstIMU()->getLinearAcceleration(); }
        
        inline virtual math::Vector3Ptr getAngularRate(const ::Ice::Current&c)
        { return getFirstIMU()->getAngularRate(); }
        
        inline virtual math::Vector3Ptr getOrientation(const ::Ice::Current&c)
        { return getFirstIMU()->getOrientation(); }
        
        inline virtual void applyForcesAndTorques(const ::Ice::Current&c)
        { /* do nothing */ }
        
        inline virtual void safeThrusters(const ::Ice::Current&c)
        { std::cout << "Thrusters safed" << std::endl; }
        
        inline virtual void unsafeThrusters(const ::Ice::Current&c)
        { std::cout << "Thrusters unsafed" << std::endl; }
        
        inline virtual void dropMarker(const ::Ice::Current&c)
        { std::cout << "Marker dropped" << std::endl; }
    };
}

#endif
