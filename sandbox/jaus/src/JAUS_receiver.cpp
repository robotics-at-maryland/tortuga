#include <iostream>

#include "JAUS_helpers.h"

#define VEHICLE_SUBSYSTEM_ID 100

int main (int argc, char * const argv[]) {
    while (true) {
        std::cout << "Response: " << receiveJAUS(VEHICLE_SUBSYSTEM_ID) << std::endl;
    }
    return 0;
}
