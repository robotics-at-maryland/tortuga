/* THIS WILL INCLUDE LOTS OF INFORMATION SUPER FUTURE MAGIC! */

/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Steve Moskovchenko <stevenm@umd.edu>
 * All rights reserved.
 *
 * Author: Steve Moskovchenko <stevenm@umd.edu>
 * File:  packages/imu/include/imuapi.h
 */

#ifndef RAM_DVLAPI_H_12_02_2009
#define RAM_DVLAPI_H_12_02_2009

// If we are compiling as C++ code we need to use extern "C" linkage
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* I'm currently guessing at what you'll need Kit */
typedef struct _RawDVLData
{
    /* not sure what these two will be used for, but they were in the IMU */
    int messageID;
    int sampleTimer;

    double bottomTrack;

    int checksumValid;
} RawDVLData;

/** Opens a serial channel to the imu using the given devices
 *
 *  @param  devName  Device filename
 *
 *  @return  The file descriptor of the device file, -1 on error.
 */
int openDVL(const char * devName);

/** Read the latest DVL measurements into the given structure
 *
 *  @param fd  The device file returned by openDVL
 */
int readDVLData(int fd, RawDVLData * imu);

// If we are compiling as C++ code we need to use extern "C" linkage
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
    
#endif // RAM_DVLAPI_H_12_02_2009
