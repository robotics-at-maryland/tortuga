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

    /*all the following angles are in terms of degrees
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
