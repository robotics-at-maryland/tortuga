

struct imuMeasurements
{
    int messageID;
    int sampleTimer;
    double gyroX;
    double gyroY;
    double gyroZ;
    double accelX;
    double accelY;
    double accelZ;
    double magX;
    double magY;
    double magZ;
    double tempX;
    double tempY;
    double tempZ;
    int checksumValid;
};
