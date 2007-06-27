// STD Includes
#include <iostream>

// Library Includes
#include "include/Config.h"

using namespace std;
int main()
{
    ConfigNode cf;

    cout << "Test Mapping: " << cf["John"]["Larry"].asInt() << endl;
    cout << "Test String: " << cf.asString() << endl;
    cout << "Test Indexing: " << cf[2].asInt() << endl;

    return 0;
}
