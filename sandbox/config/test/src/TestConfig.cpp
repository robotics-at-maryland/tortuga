// STD Includes
#include <iostream>

// Library Includes
#include "include/Config.h"

using namespace std;
int main()
{
    IConfigNode* cf = new ConfigNode();

    cout << "Test Mapping: " << cf->map("John")->map("Larry")->asInt() << endl;
    cout << "Test String: " << cf->asString() << endl;
    cout << "Test Indexing: " << cf->idx(2)->asInt() << endl;

    return 0;
}
