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
    usleep(100 * 1000);
}

int writeData(int fd, unsigned char * buf, int nbytes)
{
    return write(fd, buf, nbytes);
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
        writeData(fd, &b, 1);
        miniSleep();

        b[0]=0;

        if(hasData(fd))
            readData(fd, &b, 1);

        miniSleep();

        if(!hasData(fd) && b[0]==0xBC)
            return 0;
    }

    return -1;
}


int pingBoard(int fd)
{

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

