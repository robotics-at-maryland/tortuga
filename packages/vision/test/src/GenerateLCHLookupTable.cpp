
// STD Includes
#include <iostream>

// Project Includes
#include "vision/include/Convert.h"

using namespace ram::vision;

int main(int argc, char* argv[])
{
    std::cout << "Creating lookup table..." << std::endl
              << "This will take awhile." << std::endl;
    Convert::createLookupTable();
    Convert::saveLookupTable("lchlookup_table");
    std::cout << "Finished!" << std::endl;

    return 0;
}
