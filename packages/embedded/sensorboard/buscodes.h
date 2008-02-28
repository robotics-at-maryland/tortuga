/*
 * Commands Master can send to Slaves. Command numbers among different Slaves can overlap
 * but common commands like Ping/Identify should be the same everywhere.
 * Kind of obvious what order I did these in, huh?
 */
#define BUS_CMD_PING            0
#define BUS_CMD_ID              1
#define BUS_CMD_READ_REG        2
#define BUS_CMD_WRITE_REG       3
#define BUS_CMD_MARKER1         4
#define BUS_CMD_DEPTH           5
#define BUS_CMD_LCD_WRITE       6
#define BUS_CMD_LCD_REFRESH     7
#define BUS_CMD_LCD_LIGHT_ON    8
#define BUS_CMD_LCD_LIGHT_OFF   9
#define BUS_CMD_THRUSTERS_ON    10
#define BUS_CMD_THRUSTERS_OFF   11
#define BUS_CMD_MARKER2         12
#define BUS_CMD_CHECKWATER      14
#define BUS_CMD_TEMP            15
#define BUS_CMD_BOARDSTATUS     16
#define BUS_CMD_HARDKILL        17
#define BUS_CMD_LCD_LIGHT_FLASH 18

#define BUS_CMD_THRUSTER1_OFF   19
#define BUS_CMD_THRUSTER2_OFF   20
#define BUS_CMD_THRUSTER3_OFF   21
#define BUS_CMD_THRUSTER4_OFF   22

#define BUS_CMD_THRUSTER5_OFF   35
#define BUS_CMD_THRUSTER6_OFF   36

#define BUS_CMD_THRUSTER5_ON    37
#define BUS_CMD_THRUSTER6_ON    38

#define BUS_CMD_THRUSTER1_ON    23
#define BUS_CMD_THRUSTER2_ON    24
#define BUS_CMD_THRUSTER3_ON    25
#define BUS_CMD_THRUSTER4_ON    26
#define BUS_CMD_THRUSTER_STATE  28

#define BUS_CMD_SONAR           27

#define BUS_CMD_CLEARU1RX       29
#define BUS_CMD_CLEARU2RX       30

/* Followed by 2 parameters. MSB, LSB
 * Command goes out when LSB comes in.
 */
#define BUS_CMD_SETSPEED_U1     31
#define BUS_CMD_SETSPEED_U2     32

/* Returns 1 byte length followed by 1st byte of response
 * TODO: This should be better, but right now, motor controllers
 * only reply with 06 or nothing
 */
#define BUS_CMD_GETREPLY_U1     33
#define BUS_CMD_GETREPLY_U2     34


/* Last is 39 */


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