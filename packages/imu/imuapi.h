struct imuMeasurements
{
    int messageID;
    int sampleTimer;
    double gyroX;   /* Degrees per second */
    double gyroY;   /* Degrees per second */
    double gyroZ;   /* Degrees per second */

    double accelX;  /* Gs   */
    double accelY;  /* Gs   */
    double accelZ;  /* Gs   */

    double magX;    /* Gauss */
    double magY;    /* Gauss */
    double magZ;    /* Gauss */

    double tempX;   /* Degrees C */
    double tempY;   /* Degrees C */
    double tempZ;   /* Degrees C */
    int checksumValid;

    /*all the following angles IN RADIANS
    different axes in the variable names tell which axis the angle of the sub
     is measured with respect to*/
    double angleAccX;
    double angleAccY;
    double angleAccZ;
    double angleMagX;
    double angleMagY;
    double angleMagZ;
};



int openIMU(const char * devName);
int readIMUData(int fd, struct imuMeasurements * imu);
