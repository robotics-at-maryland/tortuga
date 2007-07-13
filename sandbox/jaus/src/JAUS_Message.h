/*
 *  JAUS_Message.h
 *  JAUS_Parser
 *
 *  Created by Leo Singer on 7/10/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#ifndef JAUS_MESSAGE_H
#define JAUS_MESSAGE_H

#include <string>
#include <iostream>
#include <stdint.h>

/* 
#	Field Description			Type			Size
----------------------------------------------------
1	Message Properties			Unsigned Short	2	   
2	Command Code				Unsigned Short	2	   
3	Destination Instance ID		Byte			1	   
4	Destination Component ID	Byte			1	   
5	Destination Node ID			Byte			1	   
6	Destination Subsystem ID	Byte			1	   
7	Source Instance ID			Byte			1	   
8	Source Component ID			Byte			1	   
9	Source Node ID				Byte			1	   
10	Source Subsystem ID 		Byte			1	   
11	Data Control (bytes)		Unsigned Short	2	   
12	Sequence Number				Unsigned Short	2	   
	Total Bytes									16	 
*/

typedef uint8_t     JAUS_byte;
typedef int16_t     JAUS_short;
typedef int32_t     JAUS_int;
typedef int64_t     JAUS_long;
typedef uint16_t    JAUS_ushort;
typedef uint32_t    JAUS_uint;
typedef uint64_t    JAUS_ulong;
typedef float       JAUS_float;
typedef double      JAUS_longfloat;

#define JAUS_HEADER_LENGTH 16
#define JAUS_MAX_FRAME_LENGTH 4096
#define JAUS_MAX_DATA_LENGTH (JAUS_MAX_FRAME_LENGTH - JAUS_HEADER_LENGTH)
#define JAUS_PREFIX_STRING "JAUS01.0"
#define JAUS_PORT 3794

#define JAUSERR_OK 0
#define JAUSERR_HEADER_TOO_SHORT 1
#define JAUSERR_WRONG_PREFIX 2
#define JAUSERR_WRONG_DATA_LENGTH 3
#define JAUSERR_DATA_LENGTH_OVER_LIMIT 4

#define JAUS_CMD_RESUME 0x0004
#define JAUS_CMD_STANDBY 0x0003

#define JAUS_VERSION_20 0
#define JAUS_VERSION_30_31 1
#define JAUS_VERSION_32_33 2

#define JAUS_HANDSHAKE_NONE 0
#define JAUS_HANDSHAKE_REQUEST_ACK_NAK 1
#define JAUS_HANDSHAKE_ACK 2
#define JAUS_HANDSHAKE_NAK 3

#define JAUS_PRIORITY_DEFAULT 6

#define JAUS_SPANNING_SINGLE_PACKET 0
#define JAUS_SPANNING_FIRST_PACKET 1
#define JAUS_SPANNING_NORMAL_PACKET 2
#define JAUS_SPANNING_RETRANSMITTED_PACKET 4
#define JAUS_SPANNING_LAST_PACKET 8

class JAUS_Message {
    friend std::ostream &operator<<(std::ostream &, const JAUS_Message &);
public:
	JAUS_Message(std::string);
	~JAUS_Message();
    
	JAUS_ushort field_MessageProperties;
	JAUS_ushort field_CommandCode;
	JAUS_byte   field_DestinationInstanceID;
	JAUS_byte   field_DestinationComponentID;
	JAUS_byte   field_DestinationNodeID;
	JAUS_byte   field_DestinationSubsystemID;
	JAUS_byte   field_SourceInstanceID;
	JAUS_byte   field_SourceComponentID;
	JAUS_byte   field_SourceNodeID;
	JAUS_byte   field_SourceSubsystemID;
	JAUS_ushort field_DataControlBits;
	JAUS_ushort field_SequenceNumber;
    
    int messageProperty_version;
    bool messageProperty_experimentalMessageFlag;
    bool messageProperty_serviceConnectionFlag;
    int messageProperty_handshake;
    int messageProperty_priority;
    
    int dataControl_dataSize;
    int dataControl_spanning;
    
    int error_code;
    
    std::string raw;
    std::string header;
    std::string message_data;
    
private:
    JAUS_ushort pack_ushort(char, char);
};

#endif
