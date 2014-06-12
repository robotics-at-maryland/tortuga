/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/imu/test/include/Helper.h
 */

// STD Includes
#include <cmath>

// Project Includes
#include "imu/test/include/Helpers.h"


int convertToRaw(double data, double range)
{
    return (data * 32768.0) / ((range/2.0) * 1.5);
}

void packPacket(IMUPacket* out, RawIMUData imu)
{
    out->gyroX = convertToRaw(imu.gyroX * RADIANS_TO_DEGS, 600);
    out->gyroY = convertToRaw(imu.gyroY * RADIANS_TO_DEGS, 600);
    out->gyroZ = convertToRaw(imu.gyroZ * RADIANS_TO_DEGS, 600);

    out->accelX = convertToRaw(imu.accelX, 4);
    out->accelY = convertToRaw(imu.accelY, 4);
    out->accelZ = convertToRaw(imu.accelZ, 4);

    out->magX = convertToRaw(imu.magX, 1.9);
    out->magY = convertToRaw(imu.magY, 1.9);
    out->magZ = convertToRaw(imu.magZ, 1.9);

    int sum = 0;
    for(int i = 0; i < sizeof(IMUPacket); i++)
        sum += ((char*)out)[i];

    sum += 0xFF * 4;

    out->checksum = sum;
}
