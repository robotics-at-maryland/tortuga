/*
 *  JAUS_helpers.cpp
 *  JAUS_Parser
 *
 *  Created by Leo Singer on 7/10/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#include "JAUS_helpers.h"

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
