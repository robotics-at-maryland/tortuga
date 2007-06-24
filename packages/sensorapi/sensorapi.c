#include <stdio.h>
#include <stdlib.h>
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
#include "sensorapi.h"
#include <poll.h>

#define USEC_PER_MSEC 1000

int hasData(int fd)
{
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    poll(&pfd, 1, 0);

    return pfd.revents & POLLIN;
}

void miniSleep()
{
    usleep(20 * 1000);
}

int writeData(int fd, unsigned char * buf, int nbytes)
{
    int ret = write(fd, buf, nbytes);
    if(ret
}

int readData(int fd, unsigned char * buf, int nbytes)
{
    return read(fd, buf, nbytes);
}


int syncBoard(int fd)
{
    unsigned char buf[5];

    usleep(100 * 1000);


    /* Eat the incoming buffer */
    while(hasData(fd))
        read(fd, buf, 1);

    int i;

    for(i=0; i<MAX_SYNC_ATTEMPTS; i++)
    {
        unsigned char b[1];
        b[0] = 0xFF;
        writeData(fd, b, 1);
        miniSleep();

        b[0]=0;

        if(hasData(fd))
            readData(fd, b, 1);

        miniSleep();

        if(!hasData(fd) && b[0]==0xBC)
            return 0;
    }

    return -1;
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



int displayText(int fd, int line, char * text)
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

