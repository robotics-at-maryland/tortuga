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
    printf("\nHello World\n");

    int fd = openThrusters("/dev/motor");

//    int fd = open("/home/steve/thruster.txt", O_RDWR);
    setSpeed(fd, 1, 250);

    printf("\n");
    return 0;
}

