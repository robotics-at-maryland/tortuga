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
#include "thrusterapi.h"
#include <poll.h>

int main(int argc, char ** argv)
{
    int fd = openThrusters("/dev/motor");

//    int fd = open("/dev/ttyUSB0", O_RDWR);
//    printf("\nSetting address: %d\n", writeReg(fd, 1, REG_ADDR, 4));

    int i=0;

//    int err=0;
//    int e1=0, e2=0, e3=0, e4=0;
    setReg(fd, 1, REG_TIMER, 0);
    setReg(fd, 2, REG_TIMER, 0);
    setReg(fd, 3, REG_TIMER, 0);
    setReg(fd, 4, REG_TIMER, 0);


    for(i=0; i<10; i++)
    {
        printf("\n");
        printf("\nResult 1 is: %d\n", setSpeed(fd, 1, 0));
        printf("\nResult 2 is: %d\n", setSpeed(fd, 2, 0));
        printf("\nResult 3 is: %d\n", setSpeed(fd, 3, 0));
        printf("\nResult 4 is: %d\n", setSpeed(fd, 4, 0));
    }

    fsync(fd);
    printf("\n");
    return 0;
}

