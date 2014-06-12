#include <iostream>

#include "JAUS_helpers.h"

#define VEHICLE_SUBSYSTEM_ID 100

#define RECEIVE_JAUS_SOCKET_ERROR -1
#define RECEIVE_JAUS_BIND_ERROR -2
#define RECEIVE_JAUS_RECVFROM_ERROR -3
#define RECEIVE_JAUS_IGNORE 0
#define RECEIVE_JAUS_RESUME 1
#define RECEIVE_JAUS_STANDBY 2

int main (int argc, char * const argv[]) {
    int result = RECEIVE_JAUS_IGNORE;
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
                std::cout << "resume" << std::endl;
                break;
            case RECEIVE_JAUS_STANDBY:
                std::cout << "standby" << std::endl;
                break;
        }
    }
    return 0;
}
