/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Kit Sczudlo <kitsczud@umd.edu>
 * All rights reserved.
 *
 * Author: Kit Sczudlo <kitsczud@umd.edu>
 * File:  packages/dvl/src/dvlapi.c
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

// Linux Includes (Not Sure If These Are needed)
#ifdef RAM_LINUX
  #include <bits/types.h>
  #include <linux/serial.h>
#endif // RAM_LINUX

// Project Includes
#include "dvlapi.h"

/*

double convertData(unsigned char msb, unsigned char lsb, double range)
{
    return convert16(msb, lsb) * ((range/2.0)*1.5) / 32768.0;
}
*/

/* Takes two bytes and pops out a short */
short convert16(unsigned char msb, unsigned char lsb)
{
    return ((signed short) ((msb << 8) | lsb));
}

/* This receives a single byte */
unsigned char waitByte(int fd)
{
    unsigned char rxb;
    while(read(fd, &rxb, 1) != 1)
        ;

    return rxb;
}

/* This waits for the two starting bytes of 0x7f7f */
/* I have this failing after SYNC_FAIL_BYTECOUNT */
int waitSync(int fd)
{
    int fs= 0;
    int syncLen= 0;

    while(fs < 2 && syncLen < SYNC_FAIL_BYTECOUNT) {
        if(waitByte(fd) == 0x7F) {
            fs++;
         } else {
            fs= 0;
         }
        syncLen++;
    }

    if(syncLen >= SYNC_FAIL_BYTECOUNT) {
        printf("UABLE TO SYCHRONIZE WITH DVL!\n");
        return -1;
    }

    return 0;
}

/* This reads in the data from the DVL and stores it so
   that the AI and controls guys have something to work with! */
int readDVLData(int fd, RawDVLData* dvl)
{
    /* So there's a variable number of bytes we can get:
       6  bytes for the header + (2 bytes * the 6th byte) +
       58 bytes for the fixed leader +
       60 bytes for the variable leader +
       81 bytes for the bottom track data +
       4  bytes for garbage and checksum
       = 211 bytes
       Based on that, here's a slightly oversized buffer:
       */
    unsigned char dvlData[256];

    int len, i, sum, tempsize, offset;
    CompleteDVLPacket *dbgpkt;

    if(waitSync(fd))
        return ERR_NOSYNC;

    /* This checks that we have the debugging packet setup! */
    if(dvl->privDbgInf == NULL) {
        printf("WARNING! Debug info reallocated!");
        dbgpkt= dvl->privDbgInf = malloc(sizeof(CompleteDVLPacket));

        /* We'll need to set up some parts of the packet... */
        dbgpkt->fixedleaderset= 0;
        dbgpkt->header.offsets= NULL;
    }

    /* We got these in the waitSync() call */
    dvlData[0]= dvlData[1]= 0x7F;

    /* Set length based on the 0x7F7F we recieved */
    len= 2;

    /* Get the header */
    while(len < 6)
        len += read(fd, dvlData + len, 6 - len);

    dbgpkt->header.HeaderID= dvlData[0];
    dbgpkt->header.DataSourceID= dvlData[1];
    dbgpkt->header.PacketSize= convert16(dvlData[3], dvlData[2]);
    /* dvlData[4] is an empty byte! */
    dbgpkt->header.num_datatypes= dvlData[5];

    /* We need to prevent memory leaks! Free up the old offset array! */
    if(dbgpkt->header.offsets != NULL)
        free(dbgpkt->header.offsets);

    dbgpkt->header.offsets= malloc(sizeof(int) * dvlData[5]);

    tempsize= 6 + ((int) dvlData[5]) * 2;
    while(len < tempsize)
        len += read(fd, dvlData + len, tempsize - len);

    for(i= 0;i < tempsize - 6;i += 2)
        dbgpkt->header.offsets[i]= convert16(dvlData[7 + i * 2],
                                             dvlData[6 + i * 2]);

    /* WOO! We've now finished reading in the header */
    /* We should probably double check that nothing is too weird... */
    if(dbgpkt->header.PacketSize > 256) {
        /* If we're here we're fucked.  We have a packet which is */
        /* too big for our buffer to hold it! */
        printf("ERROR! Packet too big!\n");
        return ERR_TOOBIG;
    }

    /*************************************************/
    /* Now we muck our way through the fixed leader! */
    /*************************************************/
    
    /* We now have an unknown offset into the buffer we'll need to */
    /* keep track of said offset. */
    offset= tempsize;

    /* The fixed leader will add an extra 58 bytes of useless info */
    tempsize+= 58;
    while(len < tempsize)
        len += read(fd, dvlData + len, tempsize - len);

    dbgpkt->fixedleader.FixedLeaderID= convert16(dvlData[offset + 1],
                                                 dvlData[offset]);
    dbgpkt->fixedleader.CPU_Firmware_Version= dvlData[offset + 2];
    dbgpkt->fixedleader.CPU_Firmware_Revision= dvlData[offset + 3];
    dbgpkt->fixedleader.System_Config= convert16(dvlData[offset + 5],
                                                 dvlData[offset + 4]);
    dbgpkt->fixedleader.Real_Sim_flag= dvlData[offset + 6];


    return 0;

    /*
    unsigned char imuData[34];

    waitSync(fd);

    int len = 0;
    int i=0, sum=0;
    while(len < 34)
        len += read(fd, imuData+len, 34-len);

    imu->messageID = imuData[0];
    imu->sampleTimer = (imuData[3]<<8) | imuData[4];

    imu->gyroX = convertData(imuData[9], imuData[10], 600) * DEGS_TO_RADIANS;
    imu->gyroY = convertData(imuData[11], imuData[12], 600) * DEGS_TO_RADIANS;
    imu->gyroZ = convertData(imuData[13], imuData[14], 600) * DEGS_TO_RADIANS;

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

    if(!imu->checksumValid)
        printf("WARNING! IMU Checksum Bad!\n");
    
    return imu->checksumValid;
    */

    /** always fails */
    return 0;
}

/* Some code from cutecom, which in turn may have come from minicom */
int openDVL(const char* devName)
{
   int fd = open(devName, O_RDWR, O_ASYNC);

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
