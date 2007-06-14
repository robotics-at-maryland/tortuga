#ifndef TEST_DEVICE_H
#define TEST_DEVICE_H

#include <string>

namespace vehicle {
namespace device {

class Device
{
public:
    virtual std::string getName() = 0;
};

}
}
    
#endif // TEST_DEVICE_H
