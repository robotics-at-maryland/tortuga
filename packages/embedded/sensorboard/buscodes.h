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

#define BUS_CMD_THRUSTER1_ON    23
#define BUS_CMD_THRUSTER2_ON    24
#define BUS_CMD_THRUSTER3_ON    25
#define BUS_CMD_THRUSTER4_ON    26

#define BUS_CMD_SONAR           27

#define BUS_CMD_THRUSTER_STATE  28
