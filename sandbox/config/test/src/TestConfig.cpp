// STD Includes
#include <iostream>

// Library Includes
#include "include/Config.h"

using namespace std;
int main()
{
    ConfigNode cf(ConfigNode::construct("Test", 0));

    cout << "Test Mapping: " << cf["John"]["Larry"].asInt() << endl;
    cout << "Test String: " << cf.asString() << endl;
    cout << "Test Indexing: " << cf[2].asInt() << endl;

    // Now lets try python with the following config:
    /*
      Modules:
          Controller:
              Param: 5
              Param2: 10
          Thrusters:
              params: [1,2,3]
              name: Port
     */

    ConfigNode cfg(ConfigNode::construct("Python", testDict()));

    cout << "cfg['Modules']['Controller']['Param'] = "
         << cfg["Modules"]["Controller"]["Param"].asInt() << endl;

    ConfigNode thruster(cfg["Modules"]["Thrusters"]);
    cout << "thruster['params'][2] =  " << thruster["params"][2].asInt() << endl;
    cout << "thruster['name'] =  " << thruster["name"].asString() << endl;

    return 0;
}
