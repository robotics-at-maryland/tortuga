#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
    int i=0, sum=0;
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

    for(i=0; i<33; i++)
        sum+=imuData[i];

    sum += 0xFF * 4;

    imu->checksumValid = (imuData[33] == (sum&0xFF));


    imu->angleMagX=(atan2(imu->magY, imu->magX)*(180.0/M_PI));
    imu->angleMagY=(atan2(imu->magZ, imu->magY)*(180.0/M_PI));
    imu->angleMagZ=(atan2(imu->magX, imu->magZ)*(180.0/M_PI));

    imu->angleAccX=(atan2(imu->accelY, imu->accelX)*(180.0/M_PI));
    imu->angleAccY=(atan2(imu->accelZ, imu->accelY)*(180.0/M_PI));
    imu->angleAccZ=(atan2(imu->accelX, imu->accelZ)*(180.0/M_PI));

    return imu->checksumValid;
}
