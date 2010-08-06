/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Steve Moskovchenko <stevenm@umd.edu>
 * All rights reserved.
 *
 * Author: Steve Moskovchenko <stevenm@umd.edu>
 * File:  packages/drivers/bfin_spartan/include/report.h
 */

#ifndef RAM_DRIVER_BFIN_REPORT_07_19_2008
#define RAM_DRIVER_BFIN_REPORT_07_19_2008

#ifdef __cplusplus
extern "C" {
#endif

#define byte unsigned char
#define uint32_t unsigned int
#define uint16_t unsigned short

/** Must be called once on startup to obtain device fd */
int openDevice();

/** Close at the end of program run */
int closeDevice(int fd);


/** Can be called many times with the same device fd */
int reportPing(int fd, byte status, double vectorX, double vectorY, double vectorZ,
               uint16_t range, uint32_t timeStamp, uint32_t sampleNo, byte pingerID);


#ifdef __cplusplus
} //extern "C" {
#endif

#endif // RAM_DRIVER_BFIN_REPORT_07_19_2008
