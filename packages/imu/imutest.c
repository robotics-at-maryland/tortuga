#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "imuapi.h"



unsigned char waitByte(int fd)
{
    unsigned char rxb[1];
    while(read(fd, rxb, 1) != 1);
    return rxb[0];
}

int waitSync(int fd)
{
    int fs=0;
    while(fs != 4)
    {
        if(waitByte(fd) == 0xFF)
            fs++;
        else
            fs=0;
    }
}


int convert16(unsigned char msb, unsigned char lsb)
{
    return (signed short) ((msb<<8)|lsb);
}

double convertData(unsigned char msb, unsigned char lsb, double range)
{
    return convert16(msb, lsb) * ((range/2.0)*1.5) / 32768.0;
}

int readIMUData(int fd, struct imuMeasurements * imu)
{
    unsigned char imuData[34];

    int len = 0;
    while(len < 34)
        len += read(fd, imuData+len, 1);

    imu->messageID = imuData[0];
    imu->sampleTimer = (imuData[3]<<8) | imuData[4];

    imu->gyroX = convertData(imuData[9], imuData[10], 600);
    imu->gyroY = convertData(imuData[11], imuData[12], 600);
    imu->gyroZ = convertData(imuData[13], imuData[14], 600);

    imu->accelX = convertData(imuData[15], imuData[16], 4);
    imu->accelY = convertData(imuData[17], imuData[18], 4);
    imu->accelZ = convertData(imuData[19], imuData[20], 4);

    imu->magX = convertData(imuData[21], imuData[22], 1.9);
    imu->magY = convertData(imuData[23], imuData[24], 1.9);
    imu->magZ = convertData(imuData[25], imuData[26], 1.9);

    imu->tempX = (((convert16(imuData[27], imuData[28])*5.0)/32768.0)/0.0084)+25.0;
    imu->tempY = (((convert16(imuData[29], imuData[30])*5.0)/32768.0)/0.0084)+25.0;
    imu->tempZ = (((convert16(imuData[31], imuData[32])*5.0)/32768.0)/0.0084)+25.0;

    return 0;
}



int main(int argc, char ** argv)
{
    int fd = open("/dev/ttyUSB0", O_RDWR); // | O_NDELAY | O_NOCTTY);

    struct imuMeasurements imuData;

    while(1)
    {
      //  printf("\nWaiting for sync...\n");
        waitSync(fd);
        readIMUData(fd, &imuData);
        //printf("%9f %9f %9f\n", imuData.accelX, imuData.accelY, imuData.accelZ);
        printf("%9f %9f %9f\n", imuData.gyroX, imuData.gyroY, imuData.gyroZ);
//        printf("%9f\n", imuData.gyroX);
    }



    printf("\nIMU Test\n");
    return 0;
}
