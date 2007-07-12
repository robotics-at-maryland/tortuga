/*
 *  JAUS_Message.cpp
 *  JAUS_Parser
 *
 *  Created by Leo Singer on 7/10/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#include "JAUS_Message.h"

#include <string>
#include <iostream>
#include <iomanip>

std::ostream &operator<<(std::ostream &out, const JAUS_Message &message) {
    out << std::left << std::setw(30) << "Parser error code: " << message.error_code << std::endl;
    out << std::endl;
    out << "HEADER FIELDS" << std::endl;
    out << std::setw(30) << "01 Message Properties " << std::hex << ((unsigned int) message.field_MessageProperties) << std::endl;
    out << std::setw(30) << "02 Command Code " << std::hex << ((unsigned int) message.field_CommandCode) << std::endl;
    out << std::setw(30) << "03 Destination Instance ID " << std::hex << ((unsigned int) message.field_DestinationInstanceID) << std::endl;
    out << std::setw(30) << "04 Destination Component ID " << std::hex << ((unsigned int) message.field_DestinationComponentID) << std::endl;
    out << std::setw(30) << "05 Destination Node ID " << std::hex << ((unsigned int) message.field_DestinationNodeID) << std::endl;
    out << std::setw(30) << "06 Destination Subsystem ID " << std::hex << ((unsigned int) message.field_DestinationSubsystemID) << std::endl;
    out << std::setw(30) << "07 Source Instance ID " << std::hex << ((unsigned int) message.field_SourceInstanceID) << std::endl;
    out << std::setw(30) << "08 Source Component ID " << std::hex << ((unsigned int) message.field_SourceComponentID) << std::endl;
    out << std::setw(30) << "09 Source Node ID " << std::hex << ((unsigned int) message.field_SourceNodeID) << std::endl;
    out << std::setw(30) << "10 Source Subsystem ID " << std::hex << ((unsigned int) message.field_SourceSubsystemID) << std::endl;
    out << std::setw(30) << "11 Data Control Bits " << std::hex << ((unsigned int) message.field_DataControlBits) << std::endl;
    out << std::setw(30) << "12 Sequence Number " << std::hex << ((unsigned int) message.field_SequenceNumber) << std::endl;
    out << std::endl;
    out << "MESSAGE PROPERTIES" << std::endl;
    out << std::setw(30) << "Version" << message.messageProperty_version << std::endl;
    out << std::setw(30) << "Experimental" << message.messageProperty_experimentalMessageFlag << std::endl;
    out << std::setw(30) << "Service" << message.messageProperty_serviceConnectionFlag << std::endl;
    out << std::setw(30) << "Handshake" << message.messageProperty_handshake << std::endl;
    out << std::setw(30) << "Priority" << message.messageProperty_priority << std::endl;
    out << std::endl;
    out << "DATA CONTROL PARAMETERS" << std::endl;
    out << std::setw(30) << "Data Size" << message.dataControl_dataSize << std::endl;
    out << std::setw(30) << "Spanning" << message.dataControl_spanning << std::endl;
    out << std::endl;
    out << "START MESSAGE DATA" << std::endl;
    out << message.message_data << std::endl;
    out << "END MESSAGE DATA" << std::endl;
    return out;
}

JAUS_Message::JAUS_Message(std::string message) : raw(message) {
    error_code = JAUSERR_OK;
    
    std::string required_prefix_string(JAUS_PREFIX_STRING);
    int prefix_strlen = required_prefix_string.length();
    int min_header_strlen = prefix_strlen + JAUS_HEADER_LENGTH;
    
    if (raw.length() < min_header_strlen) {
        error_code = JAUSERR_HEADER_TOO_SHORT;
    } else if (raw.compare(0, prefix_strlen, required_prefix_string) != 0) {
        error_code = JAUSERR_WRONG_PREFIX;
    } else {
        header = raw.substr(prefix_strlen, JAUS_HEADER_LENGTH);
        message = raw.substr(prefix_strlen + JAUS_HEADER_LENGTH);
        
        field_MessageProperties = pack_ushort(header[0], header[1]);
        field_CommandCode = pack_ushort(header[2], header[3]);
        field_DestinationInstanceID = header[4];
        field_DestinationComponentID = header[5];
        field_DestinationNodeID = header[6];
        field_DestinationSubsystemID = header[7];
        field_SourceInstanceID = header[8];
        field_SourceComponentID = header[9];
        field_SourceNodeID = header[10];
        field_SourceSubsystemID = header[11];
        field_DataControlBits = pack_ushort(header[12], header[13]);
        field_SequenceNumber = pack_ushort(header[14], header[15]);
        
        messageProperty_version = (field_MessageProperties & 0x3F00) >> 8;
        messageProperty_experimentalMessageFlag = (field_MessageProperties & 0x00F0) >> 7;
        messageProperty_serviceConnectionFlag = (field_MessageProperties & 0x0040) >> 6;
        messageProperty_handshake = (field_MessageProperties & 0x0030) >> 4;
        messageProperty_priority = (field_MessageProperties & 0x000F);
        
        dataControl_dataSize = (field_DataControlBits & 0x07FF);
        dataControl_spanning = (field_DataControlBits & 0xF000) >> 12;
        
        if (dataControl_dataSize > JAUS_MAX_DATA_LENGTH) {
            error_code = JAUSERR_DATA_LENGTH_OVER_LIMIT;
        }
        
        if (message_data.length() != dataControl_dataSize) {
            error_code = JAUSERR_WRONG_DATA_LENGTH;
        }
    }
}

JAUS_Message::~JAUS_Message() {
    
}

JAUS_ushort JAUS_Message::pack_ushort(char field1, char field2) {
    return (((JAUS_ushort) field2) << 8) | ((JAUS_ushort) field1);
}
