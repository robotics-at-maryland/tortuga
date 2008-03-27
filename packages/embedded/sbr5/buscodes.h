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


/* Battery enables on the balancer board */
#define BUS_CMD_BATT1_OFF       0x28
#define BUS_CMD_BATT2_OFF       0x29
#define BUS_CMD_BATT3_OFF       0x2A
#define BUS_CMD_BATT4_OFF       0x2B
#define BUS_CMD_BATT5_OFF       0x2C

#define BUS_CMD_BATT1_ON        0x2D
#define BUS_CMD_BATT2_ON        0x2E
#define BUS_CMD_BATT3_ON        0x2F
#define BUS_CMD_BATT4_ON        0x30
#define BUS_CMD_BATT5_ON        0x31


/* Bars (LED and fan) on distro board */
#define BUS_CMD_BAR1_OFF        0x32
#define BUS_CMD_BAR2_OFF        0x33
#define BUS_CMD_BAR3_OFF        0x34
#define BUS_CMD_BAR4_OFF        0x35
#define BUS_CMD_BAR5_OFF        0x36
#define BUS_CMD_BAR6_OFF        0x37
#define BUS_CMD_BAR7_OFF        0x38
#define BUS_CMD_BAR8_OFF        0x39

#define BUS_CMD_BAR1_ON         0x3A
#define BUS_CMD_BAR2_ON         0x3B
#define BUS_CMD_BAR3_ON         0x3C
#define BUS_CMD_BAR4_ON         0x3D
#define BUS_CMD_BAR5_ON         0x3E
#define BUS_CMD_BAR6_ON         0x3F
#define BUS_CMD_BAR7_ON         0x40
#define BUS_CMD_BAR8_ON         0x41

#define BUS_CMD_BAR_STATE       0x42
#define BUS_CMD_READ_OVR        0x43

#define BUS_CMD_READ_IMOTOR     0x44

#define BUS_CMD_READ_ASTATUS    0x45

#define BUS_CMD_BATTSTATE       0x46

/* Next free is 0x47 */










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


#define HOST_CMD_BARS               0x15

#define HOST_CMD_BARSTATE           0x16
#define HOST_REPLY_BARSTATE         0x17

#define HOST_CMD_IMOTOR             0x18
#define HOST_REPLY_IMOTOR           0x19

#define HOST_CMD_VLOW               0x1A
#define HOST_REPLY_VLOW             0x1B

#define HOST_CMD_READ_OVR           0x1C
#define HOST_REPLY_OVR              0x1D

#define HOST_CMD_BATTSTATE          0x1E
#define HOST_REPLY_BATTSTATE        0x1F

#define HOST_CMD_BATTCTL            0x20

#define CMD_BAR1_OFF     0x00
#define CMD_BAR2_OFF     0x01
#define CMD_BAR3_OFF     0x02
#define CMD_BAR4_OFF     0x03
#define CMD_BAR5_OFF     0x04
#define CMD_BAR6_OFF     0x05
#define CMD_BAR7_OFF     0x06
#define CMD_BAR8_OFF     0x07

#define CMD_BAR1_ON    0x08
#define CMD_BAR2_ON    0x09
#define CMD_BAR3_ON    0x0A
#define CMD_BAR4_ON    0x0B
#define CMD_BAR5_ON    0x0C
#define CMD_BAR6_ON    0x0D
#define CMD_BAR7_ON    0x0E
#define CMD_BAR8_ON    0x0F



#define CMD_BATT1_OFF     0x00
#define CMD_BATT2_OFF     0x01
#define CMD_BATT3_OFF     0x02
#define CMD_BATT4_OFF     0x03
#define CMD_BATT5_OFF     0x04

#define CMD_BATT1_ON      0x05
#define CMD_BATT2_ON      0x06
#define CMD_BATT3_ON      0x07
#define CMD_BATT4_ON      0x08
#define CMD_BATT5_ON      0x09
