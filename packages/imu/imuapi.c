#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <bits/types.h>
//#include <bits/termios.h>
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
    int crap=0;
    while(fs != 4)
    {
        if(waitByte(fd) == 0xFF)
            fs++;
        else
            fs=0;
        crap++;
    }
    printf("%d  ", crap);
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

    waitSync(fd);

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

/*
    imu->angleMagX=(atan2(imu->magY, imu->magX)*(180.0/M_PI));
    imu->angleMagY=(atan2(imu->magZ, imu->magY)*(180.0/M_PI));
    imu->angleMagZ=(atan2(imu->magX, imu->magZ)*(180.0/M_PI));

    imu->angleAccX=(atan2(imu->accelY, imu->accelX)*(180.0/M_PI));
    imu->angleAccY=(atan2(imu->accelZ, imu->accelY)*(180.0/M_PI));
    imu->angleAccZ=(atan2(imu->accelX, imu->accelZ)*(180.0/M_PI));
*/

    return imu->checksumValid;
}

/*
int openIMU(const char * devName)
{
    int fd = open(devName, O_RDWR, O_ASYNC); // | O_ASYNC); //, O_RDWR, O_NONBLOCK);

    if(fd == -1)
        return -1;

    printf("FD=%d\n", fd);

    struct termios info;

    memset((void *)&info, 0, sizeof(struct termios));      // clear all
    tcgetattr(fd, &info);
    cfsetispeed(&info, B115200); //SPEED_DATA[speed].cflag);
    cfsetospeed(&info, B115200); //SPEED_DATA[speed].cflag);
 //   info.c_cflag &= ~(CSIZE|PARENB|PARODD|HUPCL|CSTOPB);
  //  info.c_cflag = 0;
 //   info.c_iflag &= ~(ISTRIP|INPCK|IGNCR|INLCR|ICRNL|IXOFF|IXON);
 //   info.c_iflag |= iflag;
  //  info.c_oflag &= ~OPOST;
  //  info.c_lflag &= ~(ICANON|ECHO|ECHONL|ISIG|IEXTEN|TOSTOP);
 //   info.c_cc[VMIN]  = 0;               // wait for 1 char or timeout
    //info.c_cc[VTIME] = readTimeout/100; // wait for in deciseconds


   // info.c_ispeed = B115200;
  //  info.c_ospeed = B115200;
    printf("\nReturn is %d\n", tcsetattr(fd, TCSANOW, &info));
    return fd;
}*/



/* Some code from cutecom, which in turn may have come from minicom */
int openIMU(const char * devName)
{
   int fd = open(devName, O_RDWR, O_ASYNC); // | O_ASYNC); //, O_RDWR, O_NONBLOCK);

    if(fd == -1)
        return -1;

    printf("FD=%d\n", fd);
    struct termios newtio;
    if (tcgetattr(fd, &newtio)!=0)
        printf("\nFirst stuff failed\n");

    unsigned int _baud=B115200;
    cfsetospeed(&newtio, _baud);
    cfsetispeed(&newtio, _baud);


    newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS8;
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~(PARENB | PARODD);

    newtio.c_cflag &= ~CRTSCTS;
    newtio.c_cflag &= ~CSTOPB;

    newtio.c_iflag=IGNBRK;
    newtio.c_iflag &= ~(IXON|IXOFF|IXANY);

    newtio.c_lflag=0;

    newtio.c_oflag=0;


    newtio.c_cc[VTIME]=1;
    newtio.c_cc[VMIN]=60;

//   tcflush(m_fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &newtio)!=0)
        printf("tsetaddr1 failed!\n");

    int mcs=0;

    ioctl(fd, TIOCMGET, &mcs);
    mcs |= TIOCM_RTS;
    ioctl(fd, TIOCMSET, &mcs);

    if (tcgetattr(fd, &newtio)!=0)
        printf("tcgetattr() 4 failed\n");

    newtio.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &newtio)!=0)
      printf("tcsetattr() 2 failed\n");



    return fd;
}
