

struct imuMeasurements
{
    int messageID;
    int sampleTimer; /* 2.1701e-6 seconds per sample */

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
};
