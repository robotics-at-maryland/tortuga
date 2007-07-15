/**
 * Simple starter code for JAUS Challenge
 * NOTE: the vehicle's IP address MUST be 192.168.128.100
 * and the judge's IP address MUST be 192.168.128.1.
 */

// STD Includes
#include <iostream>
#include <cstdlib>

// Project Includes
#include "JAUS_helpers.h"
#include "thrusterapi/include/thrusterapi.h"

#define VEHICLE_SUBSYSTEM_ID 100

#define RECEIVE_JAUS_SOCKET_ERROR -1
#define RECEIVE_JAUS_BIND_ERROR -2
#define RECEIVE_JAUS_RECVFROM_ERROR -3
#define RECEIVE_JAUS_IGNORE 0
#define RECEIVE_JAUS_RESUME 1
#define RECEIVE_JAUS_STANDBY 2

void setMotors(int fd, int value)
{
    for (int i = 0; i < 4; ++i)
        setSpeed(fd, i, value);
}

int main (int argc, char * const argv[]) {
    int result = RECEIVE_JAUS_IGNORE;
    int mcFD = openThrusters("/dev/motor");

    if (mcFD < 0)
    {
        std::cerr << "Could not find motor controllers" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    while (true) {
        result = receiveJAUS(VEHICLE_SUBSYSTEM_ID);
        switch (result) {
            case RECEIVE_JAUS_SOCKET_ERROR:
                std::cout << "socket error" << std::endl;
                break;
                
            case RECEIVE_JAUS_BIND_ERROR:
                std::cout << "bind error" << std::endl;
                break;
                
            case RECEIVE_JAUS_RECVFROM_ERROR:
                std::cout << "recvfrom error" << std::endl;
                break;
                
            case RECEIVE_JAUS_IGNORE:
                std::cout << "ignore packet" << std::endl;
                break;
                
            case RECEIVE_JAUS_RESUME:
            {
                std::cout << "resume" << std::endl;
                setSpeed(mcFD, 3, -900);
                setSpeed(mcFD, 4, -900);
//		setMotors(mcFD, 300);
            }
            break;
            
            case RECEIVE_JAUS_STANDBY:
            {
                std::cout << "standby" << std::endl;
                setSpeed(mcFD, 3, 0);
                setSpeed(mcFD, 4, 0);
            }
            break;
        }
    }
    return 0;
}
