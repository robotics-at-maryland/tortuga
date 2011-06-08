/*******************************

	TYPEDEF HEADER

********************************/

#ifndef TYPEDEF_H
#define TYPEDEF_H

typedef		unsigned char		BYTE;
typedef		unsigned int		WORD16;
typedef 	unsigned long		WORD32;
typedef		unsigned long long	WORD64;
typedef 	signed char			SBYTE;
typedef 	signed int			SWORD16;
typedef 	signed long			SWORD32;
typedef		signed long long	SWORD64;

typedef union {
	WORD16 N;
	struct {
		BYTE LSB;
		BYTE MSB;
	};
	struct {
		BYTE B0;
		BYTE B1;
	};
} BWORD16;

typedef union {
	SWORD16 N;
	struct {
		BYTE LSB;
		BYTE MSB;
	};
	struct {
		BYTE B0;
		BYTE B1;
	};
} BSWORD16;

typedef union {
	WORD32 N;
	struct {
		BYTE B0;
		BYTE B1;
		BYTE B2;
		BYTE B3;
	};
	struct {
		WORD16 LSW;
		WORD16 MSW;
	};
} BWORD32;

typedef union {
	WORD64 N;
	struct {
		BYTE B0;
		BYTE B1;
		BYTE B2;
		BYTE B3;
		BYTE B4;
		BYTE B5;
		BYTE B6;
		BYTE B7;
	};
} BWORD64;

typedef struct {
	WORD64 RemoteAddress;
	BYTE SignalStrength;
	unsigned AddressBroadcast:1;
	unsigned PANBroadcast:1;
	unsigned LongAddress:1;
	BYTE DataLength;
	BYTE Data[100];
} PACKET_t;

typedef struct
{
	char Command[2];
	WORD32 Value;
	union {
		unsigned Error:1;
		unsigned QueueCommand:1;
	};
	unsigned SendValueBytes:3;
} XBEE_COMMAND_t;



#define Lsb(x)	(BYTE)(x&0x00FF)
#define Msb(x)	(BYTE)((x>>8)&0x00FF)

#endif

