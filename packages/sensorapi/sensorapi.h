#define MAX_SYNC_ATTEMPTS 20

/* In msec */
#define IO_TIMEOUT  50


#define HOST_CMD_PING    0x00
#define HOST_CMD_CHECK   0x01
#define HOST_CMD_DEPTH   0x02
#define HOST_CMD_STATUS  0x04


#define LCD_BL_OFF    0
#define LCD_BL_ON     1
#define LCD_BL_FLASH  2

#define SB_OK        0
#define SB_IOERROR  -4
#define SB_BADCC    -3
#define SB_HWFAIL   -2
#define SB_ERROR    -1

int openSensorBoard(const char * devName);
int syncBoard(int fd);
int pingBoard(int fd);
int readDepth(int fd);
int hardKill(int fd);
int dropMarker(int fd, int markerNum);
int lcdBacklight(int fd, int state);
int thrusterSafety(int fd, int state);
int displayText(int fd, int line, char * text);

