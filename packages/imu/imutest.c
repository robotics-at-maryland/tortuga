#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "imuapi.h"


int main(int argc, char ** argv)
{
    int fd = openIMU("/dev/ttyUSB0");

    if(fd == -1)
    {
        printf("Could not find device!\n");
        exit(1);
    }

    struct imuMeasurements imuData;

    while(1)
    {

        readIMUData(fd, &imuData);
        printf("%9f %9f %9f %11f %11f %11f  %11f %11f %11f  \n", imuData.accelX, imuData.accelY, imuData.accelZ, imuData.gyroX, imuData.gyroY, imuData.gyroZ, imuData.magX, imuData.magY, imuData.magZ);


        //printf("%11f %11f %11f  %11f %11f %11f  \n", imuData.angleMagX, imuData.angleMagY, imuData.angleMagZ, imuData.angleAccX, imuData.angleAccY, imuData.angleAccZ);
//        printf("%11f %11f %11f\n", imuData.gyroX, imuData.gyroY, imuData.gyroZ);
//        printf("%9f\n", imuData.gyroX);
    }
    return 0;
}
