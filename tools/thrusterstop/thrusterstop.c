/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Steve Moskovchenko <stevenm@umd.edu>
 * All rights reserved.
 *
 * Author: Steve Moskovchenko <stevenm@umd.edu>
 * File:  packages/imu/src/imuapi.c
 */

// STD Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

// UNIX Includes
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <bits/types.h>

// Linux Includes
#include <linux/serial.h>

#include <poll.h>


/* Some code from cutecom, which in turn may have come from minicom */
int openPort(const char* devName)
{
   int fd = open(devName, O_RDWR, O_ASYNC); // | O_ASYNC); //, O_RDWR, O_NONBLOCK);

    if(fd == -1)
        return -1;

    printf("FD=%d\n", fd);
    struct termios newtio;
    if (tcgetattr(fd, &newtio)!=0)
        printf("\nFirst stuff failed\n");

    unsigned int _baud=B19200;
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


int hasData(int fd, int timeout)
{
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    poll(&pfd, 1, timeout);
    return pfd.revents & POLLIN;
}


void clearBuf(int fd)
{
	unsigned char buf[2];
	usleep(20 * 1000);
	
	while(hasData(fd, 100))
	{
		read(fd, buf, 1);
		printf("%c", buf[0]);
		fflush(stdout);
	}
}


int main(int argc, char ** argv)
{
	int fd = openPort("/dev/motor");
	if(fd == -1)
	{
		printf("\nCould not find device!\n");
		return 0;
	}

	printf("\nWR: %d\n", write(fd, "Y01\r\n", 5));
	clearBuf(fd);
	fsync(fd);
	printf("\nWR: %d\n", write(fd, "Y02\r\n", 5));
	clearBuf(fd);
	fsync(fd);
	printf("\nWR: %d\n", write(fd, "Y03\r\n", 5));
	clearBuf(fd);
	fsync(fd);
	printf("\nWR: %d\n", write(fd, "Y04\r\n", 5));
	clearBuf(fd);


	fsync(fd);
	return 0;
}
