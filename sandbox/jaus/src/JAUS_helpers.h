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

bool isPacketForUs(JAUS_Message, int ocu_ip_last_octet, int vehicle_ip_last_octet);

#endif
