/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Steve Moskovchenko <stevenm@umd.edu>
 * All rights reserved.
 *
 * Author: Steve Moskovchenko <stevenm@umd.edu>
 * File:  packages/drivers/bfin_spartan/src/report.c
 */

// STD Includes
#include <stdio.h>

// UNIX Includes
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>

// Project Includes
#include "drivers/bfin_spartan/include/report.h"

int openDevice()
{
   int fd = open("/dev/ttyBF1", O_RDWR, O_ASYNC); // | O_ASYNC); //, O_RDWR, O_NONBLOCK);

    if(fd == -1)
        return -1;

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

int closeDevice(int fd)
{
    close(fd);
    return 0;
}


#define PACKET_LENGTH 26
int reportPing(int fd, byte status, double vectorX, double vectorY, double vectorZ,
               uint16_t range, uint32_t timeStamp, uint32_t sampleNo)
{
    if(fd < 0)
        return -1;

    /* Packet header to sync with PIC */
    byte buf[PACKET_LENGTH] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    /* Sanity check the vector */
    if(vectorX > 1.0 || vectorX < -1.0)
        return -2;

    if(vectorY > 1.0 || vectorY < -1.0)
        return -2;

    if(vectorZ > 1.0 || vectorZ < -1.0)
        return -2;

    /* We are sending using fractionals to four places */
    signed short vX = vectorX * 10000.0;
    signed short vY = vectorY * 10000.0;
    signed short vZ = vectorZ * 10000.0;

    /* All values big-endian */
    buf[6] = (vX >> 8) & 0xFF;
    buf[7] = (vX & 0xFF);

    buf[8] = (vY >> 8) & 0xFF;
    buf[9] = (vY & 0xFF);

    buf[10] = status;

    buf[11] = 0x00; /* Sentinel byte */

    buf[12] = (vZ >> 8) & 0xFF;
    buf[13] = (vZ & 0xFF);

    buf[14] = (range >> 8) & 0xFF;
    buf[15] = (range & 0xFF);

    buf[16] = 0x00; /* Sentinel byte */

    buf[17] = (timeStamp >> 24) & 0xFF;
    buf[18] = (timeStamp >> 16) & 0xFF;
    buf[19] = (timeStamp >> 8) & 0xFF;
    buf[20] = (timeStamp & 0xFF);

    buf[21] = 0x00; /* Sentinel byte */

    buf[22] = (sampleNo >> 24) & 0xFF;
    buf[23] = (sampleNo >> 16) & 0xFF;
    buf[24] = (sampleNo >> 8) & 0xFF;
    buf[25] = (sampleNo & 0xFF);

    if(write(fd, buf, PACKET_LENGTH) == PACKET_LENGTH)
        return 0;
    else
        return -3;
}


