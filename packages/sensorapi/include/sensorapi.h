#define MAX_SYNC_ATTEMPTS 20

/* In msec */
#define IO_TIMEOUT  100


#define HOST_CMD_PING    0x00
#define HOST_CMD_CHECK   0x01
#define HOST_CMD_DEPTH   0x02
#define HOST_CMD_STATUS  0x04
#define HOST_CMD_TEMPERATURE 0x0A
#define HOST_CMD_SONAR 0x0D
#define HOST_CMD_THRUSTERSTATE 0x10


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


#define NUM_TEMP_SENSORS 5

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

/** Returns the file*/
int openSensorBoard(const char * devName);

/** Syncs the communication protocol between the board and vehicle */
int syncBoard(int fd);

int checkBoard(int fd);


int pingBoard(int fd);

int readDepth(int fd);

/** Read the status bit back from the board */
int readStatus(int fd);

int readThrusterState(int fd);

int hardKill(int fd);

/** Releases marker, only takes 0 or 1 */
int dropMarker(int fd, int markerNum);

int lcdBacklight(int fd, int state);

int thrusterSafety(int fd, int state);

int displayText(int fd, int line, const char* text);

/**  Reads the values from the board's temperature

     @param fd
         The file descriptor returned by openSensorBoard()
     @param tempData
         Where the sensor data is written. The array must be at least
         NUM_TEMP_SENSORS elements long. The temperatures are specified in
         degrees C. A value of 255 indicates a missing or malfunctioning
         sensor.
     @return SB_OK upon success or SB_ERROR.
**/
int readTemp(int fd, unsigned char * tempData);

int getSonarData(int fd, int * angle, int * distance, int * pingNumber);

int setDiagnostics(int fd, int state);

/** Set the speed of the thrusters

    This command takes about 2 ms to execute.  You must call
    readSpeedResponses before this command, or about 15 ms after this call is
    made.
    
    @param fd
         The file descriptor returned by openSensorBoard()
         
    @param s1
         The speed of thruster with address one
    @param s2
         The speed of thruster with address two
    @param s3
         The speed of thruster with address three
    @param s4
         The speed of thruster with address four
 */
int setSpeeds(int fd, int s1, int s2, int s3, int s4);

/** Reads back the on the board from the motor controller

    This is basically a house cleaning command, seee setSpeeds for information
    on its use.
 */
int readSpeedResponses(int fd);

int readThrusterState(int fd);


// If we are compiling as C++ code we need to use extern "C" linkage
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
