#ifndef TEST_VEHICLE_H
#define TEST_VEHICLE_H

#include <string>
#include <map>

class Device;

class Vehicle
{
public:
    Vehicle();
    
    Device* getDevices(std::string name);
private:
    std::map<std::string, Device*> m_devices;
};

#endif // TEST_VEHICLE_H
