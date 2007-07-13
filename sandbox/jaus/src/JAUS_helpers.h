/*
 *  JAUS_helpers.h
 *  JAUS_Parser
 *
 *  Created by Leo Singer on 7/10/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#ifndef JAUS_HELPERS_H
#define JAUS_HELPERS_H

#include "JAUS_Message.h"

#define RECEIVE_JAUS_SOCKET_ERROR -1
#define RECEIVE_JAUS_BIND_ERROR -2
#define RECEIVE_JAUS_RECVFROM_ERROR -3
#define RECEIVE_JAUS_IGNORE 0
#define RECEIVE_JAUS_RESUME 1
#define RECEIVE_JAUS_STANDBY 2

int receiveJAUS(int vehicle_ip_last_octet);

bool isPacketForUs(JAUS_Message, int ocu_ip_last_octet, int vehicle_ip_last_octet);

#endif
