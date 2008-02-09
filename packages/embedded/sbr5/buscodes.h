/*
 * Commands Master can send to Slaves. Command numbers among different Slaves can overlap
 * but common commands like Ping/Identify should be the same everywhere.
 * Kind of obvious what order I did these in, huh?
 */
#define BUS_CMD_PING            0x00
#define BUS_CMD_ID              0x01
#define BUS_CMD_READ_REG        0x02
#define BUS_CMD_WRITE_REG       0x03
#define BUS_CMD_MARKER1         0x04
#define BUS_CMD_DEPTH           0x05
#define BUS_CMD_LCD_WRITE       0x06
#define BUS_CMD_LCD_REFRESH     0x07
#define BUS_CMD_LCD_LIGHT_ON    0x08
#define BUS_CMD_LCD_LIGHT_OFF   0x09
#define BUS_CMD_THRUSTERS_ON    0x0A
#define BUS_CMD_THRUSTERS_OFF   0x0B
#define BUS_CMD_MARKER2         0x0C
#define BUS_CMD_CHECKWATER      0x0E
#define BUS_CMD_TEMP            0x0F
#define BUS_CMD_BOARDSTATUS     0x10
#define BUS_CMD_HARDKILL        0x11
#define BUS_CMD_LCD_LIGHT_FLASH 0x12

#define BUS_CMD_THRUSTER1_OFF   0x13
#define BUS_CMD_THRUSTER2_OFF   0x14
#define BUS_CMD_THRUSTER3_OFF   0x15
#define BUS_CMD_THRUSTER4_OFF   0x16
#define BUS_CMD_THRUSTER5_OFF   0x23
#define BUS_CMD_THRUSTER6_OFF   0x24


#define BUS_CMD_THRUSTER1_ON    0x17
#define BUS_CMD_THRUSTER2_ON    0x18
#define BUS_CMD_THRUSTER3_ON    0x19
#define BUS_CMD_THRUSTER4_ON    0x1A
#define BUS_CMD_THRUSTER5_ON    0x25
#define BUS_CMD_THRUSTER6_ON    0x26


#define BUS_CMD_THRUSTER_STATE  0x1C

#define BUS_CMD_SONAR           0x1B

#define BUS_CMD_CLEARU1RX       0x1D
#define BUS_CMD_CLEARU2RX       0x1E

/* Followed by 2 parameters. MSB, LSB
 * Command goes out when LSB comes in.
 */
#define BUS_CMD_SETSPEED_U1     0x1F
#define BUS_CMD_SETSPEED_U2     0x20

/* Returns 1 byte length followed by 1st byte of response
 * TODO: This should be better, but right now, motor controllers
 * only reply with 06 or nothing
 */
#define BUS_CMD_GETREPLY_U1     0x21
#define BUS_CMD_GETREPLY_U2     0x22


#define BUS_CMD_STARTSW         0x27

/* Next free is 0x28 */










#define HOST_CMD_SYNC               0xFF

#define HOST_CMD_PING               0x00
#define HOST_REPLY_SUCCESS          0xBC
#define HOST_REPLY_FAILURE          0xDF
#define HOST_REPLY_BADCHKSUM        0xCC

#define HOST_CMD_SYSCHECK           0x01

#define HOST_CMD_DEPTH              0x02
#define HOST_REPLY_DEPTH            0x03

#define HOST_CMD_BOARDSTATUS        0x04
#define HOST_REPLY_BOARDSTATUS      0x05

#define HOST_CMD_HARDKILL           0x06
#define HOST_CMD_MARKER             0x07

#define HOST_CMD_BACKLIGHT          0x08

#define HOST_CMD_THRUSTERS          0x09

#define HOST_CMD_TEMPERATURE        0x0A
#define HOST_REPLY_TEMPERATURE      0x0B

#define HOST_CMD_PRINTTEXT          0x0C

#define HOST_CMD_SONAR              0x0D
#define HOST_REPLY_SONAR            0x0E

#define HOST_CMD_RUNTIMEDIAG        0x0F

#define HOST_CMD_THRUSTERSTATE      0x10
#define HOST_REPLY_THRUSTERSTATE    0x11

#define HOST_CMD_SETSPEED           0x12

#define HOST_CMD_MOTOR_READ         0x13
#define HOST_CMD_MOTOR_REPLY        0x14
