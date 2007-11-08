/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Steve Moskovchenko <stevenm@umd.edu>
 * All rights reserved.
 *
 * Author: Steve Moskovchenko <stevenm@umd.edu>
 * File:  packages/sensorapi/src/sensorapi.c
 */

// STD Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// UNIX Includes
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <poll.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

// Linux Includes (Not Sure If These Are needed)
#ifdef RAM_LINUX
  #include <bits/types.h>
  #include <linux/serial.h>
#endif // RAM_LINUX

// Project Includes
#include "sensorapi.h"

#define USEC_PER_MSEC 1000

int hasData(int fd, int timeout)
{
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    poll(&pfd, 1, timeout);

    return pfd.revents & POLLIN;
}

void miniSleep()
{
    usleep(20 * 1000);
}

int writeData(int fd, unsigned char * buf, int nbytes)
{
    int ret = write(fd, buf, nbytes);
    return ret;
}

int readData(int fd, unsigned char * buf, int nbytes)
{
    if(!hasData(fd, IO_TIMEOUT))
        return SB_IOERROR;

    return read(fd, buf, nbytes);
}


int syncBoard(int fd)
{
    unsigned char buf[5];

    /* Eat the incoming buffer */
    while(hasData(fd, 0))
        read(fd, buf, 1);

    int i;

    for(i=0; i<MAX_SYNC_ATTEMPTS; i++)
    {
        unsigned char b[1];
        b[0] = 0xFF;
        writeData(fd, b, 1);
	    miniSleep();

        b[0]=0;

        if(hasData(fd, IO_TIMEOUT))
            readData(fd, b, 1);
	    miniSleep();

        if(!hasData(fd, IO_TIMEOUT) && b[0]==0xBC)
            return SB_OK;
    }

    return SB_ERROR;
}



int pingBoard(int fd)
{
    unsigned char buf[2]={HOST_CMD_PING, HOST_CMD_PING};
    writeData(fd, buf, 2);
    readData(fd, buf, 1);
    if(buf[0] == 0xBC)
        return SB_OK;
    return SB_HWFAIL;
}




int checkBoard(int fd)
{
    unsigned char buf[2]={HOST_CMD_CHECK, HOST_CMD_CHECK};
    writeData(fd, buf, 2);
    readData(fd, buf, 1);
    if(buf[0] == 0xBC)
        return SB_OK;

    if(buf[0] == 0xDF)
        return SB_HWFAIL;

    if(buf[0] == 0xCC)
        return SB_BADCC;

    return SB_ERROR;
}




int readDepth(int fd)
{
    unsigned char buf[5]={HOST_CMD_DEPTH, HOST_CMD_DEPTH};
    writeData(fd, buf, 2);
    readData(fd, buf, 1);
    if(buf[0] != 0x03)
        return SB_ERROR;

    readData(fd, buf, 3);

    if( ((0x03 + buf[0] + buf[1]) & 0xFF) == buf[2])
        return buf[0]<<8 | buf[1];

    return SB_ERROR;
}



int readStatus(int fd)
{
    unsigned char buf[5]={HOST_CMD_STATUS, HOST_CMD_STATUS};
    writeData(fd, buf, 2);
    readData(fd, buf, 1);
    if(buf[0] != 0x05)
        return SB_ERROR;

    readData(fd, buf, 2);

    if( ((0x05 + buf[0]) & 0xFF) == buf[1])
        return buf[0];

    return SB_ERROR;
}


int readThrusterState(int fd)
{
    unsigned char buf[5]={HOST_CMD_THRUSTERSTATE, HOST_CMD_THRUSTERSTATE};
    writeData(fd, buf, 2);
    readData(fd, buf, 1);
    if(buf[0] != 0x11)
        return SB_ERROR;

    readData(fd, buf, 2);

    if( ((0x11 + buf[0]) & 0xFF) == buf[1])
        return buf[0];

    return SB_ERROR;
}

int readTemp(int fd, unsigned char * tempData)
{
    unsigned char buf[5]={HOST_CMD_TEMPERATURE, HOST_CMD_TEMPERATURE};
    int i;
    for(i=0; i<NUM_TEMP_SENSORS; i++)
        tempData[i]=0;

    writeData(fd, buf, 2);
    readData(fd, buf, 1);
    if(buf[0] != 0x0B)
        return SB_ERROR;

    readData(fd, tempData, NUM_TEMP_SENSORS);
    readData(fd, buf, 1);

    unsigned char sum = 0x0B;

    for(i=0; i<NUM_TEMP_SENSORS; i++)
        sum = (sum+tempData[i]) & 0xFF;

    if(sum == buf[0])
        return SB_OK;

    return SB_ERROR;
}


int getSonarData(int fd, int * angle, int * distance, int * pingNumber)
{
    unsigned char buf[5]={HOST_CMD_SONAR, HOST_CMD_SONAR};
    int i;
    unsigned char rawSonar[5] = {0,0,0,0,0};

    writeData(fd, buf, 2);
    readData(fd, buf, 1);

    if(buf[0] != 0x0E)
        return SB_ERROR;

    readData(fd, rawSonar, 5);
    readData(fd, buf, 1);

    unsigned char sum = 0x0E;

    for(i=0; i<5; i++)
        sum = (sum+rawSonar[i]) & 0xFF;

    if(sum != buf[0])
        return SB_ERROR;

    printf("\nDebug: Received data from sonar board: < ");
    for(i=0; i<5; i++)
        printf("0x%02X ", rawSonar[i]);

    printf(">\n");

    *angle = 0;
    *distance = 0;
    *pingNumber = rawSonar[5];


    return SB_OK;
}


int hardKill(int fd)
{
    unsigned char buf[6]={0x06, 0xDE, 0xAD, 0xBE, 0xEF, 0x3E};
    writeData(fd, buf, 6);
    readData(fd, buf, 1);

    if(buf[0] == 0xBC)
        return SB_OK;

    if(buf[0] == 0xCC)
        return SB_BADCC;

    if(buf[0] == 0xDF)
        return SB_HWFAIL;

    return SB_ERROR;
}



int dropMarker(int fd, int markerNum)
{
    if(markerNum != 0 && markerNum != 1)
        return -255;

    unsigned char buf[3]={0x07, 0x00, 0x00};

    buf[1] = markerNum;
    buf[2] = markerNum + 0x07;


    writeData(fd, buf, 3);
    readData(fd, buf, 1);

    if(buf[0] == 0xBC)
        return SB_OK;

    if(buf[0] == 0xCC)
        return SB_BADCC;

    if(buf[0] == 0xDF)
        return SB_HWFAIL;

    return SB_ERROR;
}


int lcdBacklight(int fd, int state)
{
    if(state != LCD_BL_OFF && state != LCD_BL_ON && state != LCD_BL_FLASH)
        return -255;

    unsigned char buf[3]={0x08, 0x00, 0x00};

    buf[1] = state;
    buf[2] = state + 0x08;


    writeData(fd, buf, 3);
    readData(fd, buf, 1);

    if(buf[0] == 0xBC)
        return SB_OK;

    if(buf[0] == 0xCC)
        return SB_BADCC;

    if(buf[0] == 0xDF)
        return SB_HWFAIL;

    return SB_ERROR;
}


int thrusterSafety(int fd, int state)
{
    if(state<0 || state>7)
        return -255;

    unsigned char buf[8]={0x09, 0xB1, 0xD0, 0x23, 0x7A, 0x69, 0, 0};

    buf[6] = state;

    int i;

    for(i=0; i<7; i++)
        buf[7] += buf[i];


    writeData(fd, buf, 8);

    readData(fd, buf, 1);

    if(buf[0] == 0xBC)
        return SB_OK;

    if(buf[0] == 0xCC)
        return SB_BADCC;

    if(buf[0] == 0xDF)
        return SB_HWFAIL;

    return SB_ERROR;
}



int displayText(int fd, int line, const char* text)
{
    if(line!=0 && line!=1)
        return -255;

    if(!text)
        return 0;

    unsigned char buf[20]={0x0C, 0,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,0 };

    buf[1] = line;

    int i;

    for(i=0; text[i]!=0 && i<16; i++)
        buf[i+2]=text[i];

    for(i=0; i<18; i++)
        buf[18] += buf[i];


    writeData(fd, buf, 19);

    readData(fd, buf, 1);

    if(buf[0] == 0xBC)
        return SB_OK;

    if(buf[0] == 0xCC)
        return SB_BADCC;

    if(buf[0] == 0xDF)
        return SB_HWFAIL;

    return SB_ERROR;
}


// MSB LSB !! (big endian)
int setSpeeds(int fd, int s1, int s2, int s3, int s4)
{
    int i=0;
    unsigned char buf[10]={0x12, 0,0, 0,0, 0,0, 0,0, 0x00};

    buf[1] = (s1 >> 8);
    buf[2] = (s1 & 0xFF);

    buf[3] = (s2 >> 8);
    buf[4] = (s2 & 0xFF);

    buf[5] = (s3 >> 8);
    buf[6] = (s3 & 0xFF);

    buf[7] = (s4 >> 8);
    buf[8] = (s4 & 0xFF);

    buf[9] = 0;

    for(i=0; i<9; i++)
        buf[9]+=buf[i];

    writeData(fd, buf, 10);
    readData(fd, buf, 1);

    if(buf[0] == 0xBC)
        return SB_OK;

    if(buf[0] == 0xCC)
        return SB_BADCC;

    if(buf[0] == 0xDF)
        return SB_HWFAIL;

    return SB_ERROR;
}

// 14 xx xx xx xx CS
int readSpeedResponses(int fd)
{
    unsigned char buf[6]={0x13, 0x13};
    int i;

    writeData(fd, buf, 2);
    readData(fd, buf, 1);

    if(buf[0] != 0x14)
        return SB_ERROR;

    readData(fd, buf+1, 5);

    unsigned char sum = 0;

    for(i=0; i<5; i++)
        sum = (sum+buf[i]) & 0xFF;

    if(sum != buf[5])
        return SB_ERROR;

    int errCount=0;

    if(buf[1] != 0x06)
        errCount++;

    if(buf[2] != 0x06)
        errCount++;

    if(buf[3] != 0x06)
        errCount++;

    if(buf[4] != 0x06)
        errCount++;

    if(errCount != 0)
    {
        printf("\t Got: %02x %02x %02x %02x\n", buf[1], buf[2], buf[3], buf[4]);
        return SB_ERROR;
    }

    return SB_OK;

}


int setDiagnostics(int fd, int state)
{
    if(state != 0 && state != 1)
        return -255;

    unsigned char buf[3]={0x0F, 0x00, 0x00};

    buf[1] = state;
    buf[2] = state + 0x0F;


    writeData(fd, buf, 3);
    readData(fd, buf, 1);

    if(buf[0] == 0xBC)
        return SB_OK;

    if(buf[0] == 0xCC)
        return SB_BADCC;

    if(buf[0] == 0xDF)
        return SB_HWFAIL;

    return SB_ERROR;
}



/* Some code from cutecom, which in turn may have come from minicom */
/* FUGLY but it does what I want */
int openSensorBoard(const char * devName)
{
   int fd = open(devName, O_RDWR, O_ASYNC); // | O_ASYNC); //, O_RDWR, O_NONBLOCK);

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

