/*
 *  JAUS_helpers.cpp
 *  JAUS_Parser
 *
 *  Created by Leo Singer on 7/10/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>

#include <string>
#include <iostream>

#include "JAUS_helpers.h"
#include "JAUS_Message.h"

int receiveJAUS(int vehicle_ip_last_octet) {
    int sd, rc;
    
    struct sockaddr_in serveraddr, clientaddr;
    int clientaddrlen = sizeof(clientaddr);
    int serveraddrlen = sizeof(serveraddr);
    char buffer[JAUS_MAX_FRAME_LENGTH];
    char *bufptr = buffer;
    int buflen = sizeof(buffer);

    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        return RECEIVE_JAUS_SOCKET_ERROR;
    
    memset(&serveraddr,0x00, serveraddrlen);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(JAUS_PORT);
    serveraddr.sin_addr.s_addr= htonl(INADDR_ANY);

    if ((rc = bind(sd, (struct sockaddr *)&serveraddr, serveraddrlen)) < 0) {
        close(sd);
        return RECEIVE_JAUS_BIND_ERROR;
    }
    
    rc = recvfrom(sd, bufptr, buflen, 0, (struct sockaddr *)&clientaddr, (socklen_t*)&clientaddrlen);

    if (rc < 0) {
        close(sd);
        return RECEIVE_JAUS_RECVFROM_ERROR;
    }

    close(sd);

    std::string buf_string(buffer, rc);
    
    JAUS_Message mesg(buf_string);
	
    int ocu_ip_last_octet = (ntohl(*((int*)(&clientaddr.sin_addr))) & 0xFF);
	std::cout << "OCU IP Last Octet:" << ocu_ip_last_octet << std::endl;

    if (isPacketForUs(mesg, ocu_ip_last_octet, vehicle_ip_last_octet)) {
        if (mesg.field_CommandCode == JAUS_CMD_RESUME) {
            return RECEIVE_JAUS_RESUME;
        } else if (mesg.field_CommandCode == JAUS_CMD_STANDBY) {
            return RECEIVE_JAUS_STANDBY;
        } else {
            return RECEIVE_JAUS_IGNORE;
        }
    } else {
        return RECEIVE_JAUS_IGNORE;
    }
}

/**
 * This routine will not determine IP addresses.  However, the IP addresses must
 * be correct and some of the fields of the header must also reflect them.
 * 
 * As per the JAUS Challenge specs, 
 * the IP address of the OCU (the controller) must be 192.168.128.1.
 * The IP address of the vehicle must be between 192.168.128.2 and 192.168.128.254.
 */

bool isPacketForUs(JAUS_Message m, int ocu_ip_last_octet, int vehicle_ip_last_octet) {
    return m.error_code == 0 && 
    m.messageProperty_version == JAUS_VERSION_32_33 && 
    m.messageProperty_experimentalMessageFlag == false &&
    m.messageProperty_serviceConnectionFlag == false &&
    m.messageProperty_handshake == JAUS_HANDSHAKE_NONE &&
    m.messageProperty_priority == JAUS_PRIORITY_DEFAULT &&
    m.field_DestinationInstanceID == 1 &&
    m.field_DestinationComponentID == 33 &&
    m.field_DestinationNodeID == 1 &&
    m.field_DestinationSubsystemID == vehicle_ip_last_octet &&
    m.field_SourceInstanceID == 1 &&
    m.field_SourceComponentID == 40 &&
    m.field_SourceNodeID == 1 &&
    m.field_SourceSubsystemID == ocu_ip_last_octet &&
    m.field_SequenceNumber == 0;
}
