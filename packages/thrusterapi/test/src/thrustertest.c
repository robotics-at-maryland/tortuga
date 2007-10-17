/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Steve Moskovchenko <stevenm@umd.edu>
 * All rights reserved.
 *
 * Author: Steve Moskovchenko <stevenm@umd.edu>
 * File:  packages/thrusterapi/test/src/thrustertest.c
 */

// STD Includes
#include <stdio.h>

// Unix Includes
#include <unistd.h>

// Project Includes
#include "thrusterapi.h"

int main(int argc, char ** argv)
{
    printf("\nHello World\n");

    int fd = openThrusters("/dev/motor");

//    int fd = open("/dev/ttyUSB0", O_RDWR);
//    printf("\nSetting address: %d\n", writeReg(fd, 1, REG_ADDR, 4));

    int i=0;

    int err=0;
    int e1=0, e2=0, e3=0, e4=0;
//    setReg(fd, 1, REG_TIMER, 0);
//    setReg(fd, 2, REG_TIMER, 0);
//    setReg(fd, 3, REG_TIMER, 0);
//    setReg(fd, 4, REG_TIMER, 0);


    for(i=0; i<200; i++)
    {
        printf("\n");
//	sleep(1);
        printf("\nResult 1 is: %d\n", e1 += setSpeed(fd, 1, i*3));
        printf("\nResult 2 is: %d\n", e2 += setSpeed(fd, 2, i*3));
        printf("\nResult 3 is: %d\n", e3 += setSpeed(fd, 3, i*3));
        printf("\nResult 4 is: %d\n", e4 += setSpeed(fd, 4, i*3));
        err += e1+e2+e3+e4;
    }


    setSpeed(fd, 1, 0);
    setSpeed(fd, 2, 0);
    setSpeed(fd, 3, 0);
    setSpeed(fd, 4, 0);
    
    printf("\ne1=%d\n", e1);
    printf("\ne2=%d\n", e2);
    printf("\ne3=%d\n", e3);
    printf("\ne4=%d\n", e4);

    printf("\nerr=%d\n", err);

    fsync(fd);
    printf("\n");
    return 0;
}

