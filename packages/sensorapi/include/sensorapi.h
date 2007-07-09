#define MAX_SYNC_ATTEMPTS 20

/* In msec */
#define IO_TIMEOUT  100


#define HOST_CMD_PING    0x00
#define HOST_CMD_CHECK   0x01
#define HOST_CMD_DEPTH   0x02
#define HOST_CMD_STATUS  0x04
#define HOST_CMD_TEMPERATURE 0x0A

#define LCD_BL_OFF    0
#define LCD_BL_ON     1
#define LCD_BL_FLASH  2

#define SB_OK        0
#define SB_IOERROR  -4
#define SB_BADCC    -3
#define SB_HWFAIL   -2
#define SB_ERROR    -1


#define CMD_THRUSTER1_OFF     0
#define CMD_THRUSTER2_OFF     1
#define CMD_THRUSTER3_OFF     2
#define CMD_THRUSTER4_OFF     3
#define CMD_THRUSTER1_ON      4
#define CMD_THRUSTER2_ON      5
#define CMD_THRUSTER3_ON      6
#define CMD_THRUSTER4_ON      7


#define NUM_TEMP_SENSORS 6

/* Bits of the status command */
/* Use these constants. The values can, and most likely will, change. */
#define STATUS_WATER      0x01
#define STATUS_KILLSW	  0x02

#define STATUS_BATT1      0x04
#define STATUS_BATT2      0x08
#define STATUS_BATT3      0x10
#define STATUS_BATT4      0x20

#define STATUS_MYSTERYBIT 0x40

#define STATUS_STARTSW    0x80

// If we are compiling as C++ code we need to use extern "C" linkage
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int openSensorBoard(const char * devName);
int syncBoard(int fd);
int pingBoard(int fd);
int readDepth(int fd);
int hardKill(int fd);
int dropMarker(int fd, int markerNum);
int lcdBacklight(int fd, int state);
int thrusterSafety(int fd, int state);
int displayText(int fd, int line, char * text);
int getTemp(int fd, unsigned char * tempData);

// If we are compiling as C++ code we need to use extern "C" linkage
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
