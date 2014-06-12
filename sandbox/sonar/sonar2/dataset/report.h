#ifdef __cplusplus
extern "C" {
#endif

#define byte unsigned char
#define uint32_t unsigned int
#define uint16_t unsigned short

/* Must be called once on startup to obtain device fd */
int openDevice();

/* Close at the end of program run */
int closeDevice(int fd);


/* Can be called many times with the same device fd */
int reportPing(int fd, byte status, double vectorX, double vectorY, double vectorZ,
               uint16_t range, uint32_t timeStamp, uint32_t sampleNo);


#ifdef __cplusplus
} //extern "C" {
#endif
