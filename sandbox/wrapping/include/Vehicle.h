#ifndef TEST_VEHICLE_H
#define TEST_VEHICLE_H

#include <string>
#include <map>

namespace vehicle {
namespace device {
    class Device;
}

class Vehicle
{
public:
    Vehicle();
    
    device::Device* getDevices(std::string name);
private:
    std::map<std::string, device::Device*> m_devices;
};

}
    
#endif // TEST_VEHICLE_H
