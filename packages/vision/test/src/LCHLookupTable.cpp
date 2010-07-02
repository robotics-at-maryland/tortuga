
// STD Includes
#include <iostream>
#include <string.h>
#include <time.h>

// Project Includes
#include "vision/include/Convert.h"

using namespace ram::vision;

int main(int argc, char* argv[])
{
    if (argc < 2 || (strcmp("-h", argv[1]) == 0 ||
                     strcmp("--help", argv[1]) == 0)) {
        std::cout << "Generates and tests the LCH lookup table\n\n"
            "\t--help,-h    \t\tDisplays this message\n"
            "\t-g,--generate\t\tGenerates the lookup table\n"
            "\t-t,--test    \t\tTests the lookup table loading" << std::endl;
        return 1;
    }

    if (argc > 1 && strcmp("-g", argv[1]) == 0) {
        std::cout << "Creating lookup table..." << std::endl
                  << "This will take awhile." << std::endl;
        Convert::createLookupTable();
        std::cout << "Finished!" << std::endl;
    } else {
        // Load the lookup table and time it
        std::cout << "Testing lookup table loading speed..." << std::endl;
        clock_t start = clock();
        Convert::loadLookupTable();
        clock_t end = clock();
        std::cout << "Finished! Took " << (end - start)/(double)CLOCKS_PER_SEC
                  << " seconds" << std::endl;
    }
    return 0;
    
    return 0;
}
