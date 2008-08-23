#include "SimVehicle.h"
#include "SimDepthSensor.h"
#include "SimPowerSource.h"
#include "SimIMU.h"

namespace ram {
    namespace sim {
        SimVehicle::SimVehicle(const ::Ice::Current&c)
        : collisionShape(btVector3(0.25,0.25,0.75)), BuoyantBody(80, &collisionShape, btVector3(5, 5, 10))
        {
            setBuoyantVolume(0.1);
            setCenterOfBuoyancyPosition(btVector3(0.05,0,0.005));
            addDevice(new SimIMU(*this, "Yoyodyne Propulsion Systems IMU"), c);
            addDevice(new SimDepthSensor(*this, "Absolute Pressure Sensor"), c);
            addDevice(new SimPowerSource("Battery 1"), c);
            addDevice(new SimDepthSensor(*this, "Bottom Ranging Sonar"), c);
            addDevice(new SimPowerSource("Acme Thermonuclear Reactor"), c);
            addDevice(new SimDepthSensor(*this, "Surface Ranging Sonar"), c);
        }
    }
}
