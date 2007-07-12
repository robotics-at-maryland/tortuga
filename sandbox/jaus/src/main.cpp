#include <iostream>
#include <fstream>
#include <string>
#include "JAUS_Message.h"
#include "JAUS_helpers.h"

int main (int argc, char * const argv[]) {
    char buf[4096];
    
    int ocu_ip_last_octet = 1;
    int vehicle_ip_last_octet = 2;
    
    std::ifstream fin("./test_packet.dat", std::ios::in);
    if (!fin) {
        std::cerr << "Could not read test file." << std::endl;
        return -1;
    }
    fin.read(buf, 4096);
    int numCharsRead = fin.gcount();
    fin.close();
    
    std::cout << "RAW PACKET" << std::endl;
    std::cout << buf << std::endl << std::endl;
    
    std::string buf_string(buf, numCharsRead);
    
    JAUS_Message test_message(buf_string);
    
    std::cout << test_message << std::endl;
    
    if (isPacketForUs(test_message, ocu_ip_last_octet, vehicle_ip_last_octet)) {
        std::cout << "This message is bound for us." << std::endl;
        if (test_message.field_CommandCode == JAUS_CMD_RESUME) {
            std::cout << "RESUME command, go FORWARD." << std::endl;
        } else if (test_message.field_CommandCode == JAUS_CMD_STANDBY) {
            std::cout << "STANDBY command, ALL STOP." << std::endl;
        } else {
            std::cout << "UNRECOGNIZED command, DO NOTHING." << std::endl;
            std::cout << "This is not necessariliy an error.  It just means " <<
            "that this command is unimplemented." << std::endl;
        }
    } else {
        std::cout << "This message is NOT bound for us.  Ignore it." << std::endl;
    }
    
    return 0;
}
