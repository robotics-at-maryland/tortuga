/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/imu/test/include/Helper.h
 */

#ifndef RAM_IMU_TEST_HELPERS_09_14_2007
#ifndef RAM_IMU_TEST_HELPERS_09_14_2007

#include <boost/cstdint.hpp> // for accurate int types

namespace ram {
namespace imu {

struct IMUPacket
{
    int32_t sync;
    int8_t size;
    int8_t deviceID;
    int8_t messageID;
    int16_t time;
    int32_t reserved;
    
    int16_t gyroX;
    int16_t gyroY;
    int16_t gyroZ;

    int16_t accelX;
    int16_t accelY;
    int16_t accelZ;

    int16_t magX;
    int16_t magY;
    int16_t magZ;

    int16_t gyroTempX;
    int16_t gyroTempY;
    int16_t gyroTempZ;

    int8_t checksum;
};

    /** Takes the SI number and outputs the raw value*/
int convertToRaw(double data, double range)
{
    return (data * 32768.0) / ((range/2.0) * 1.5);
}

static const double DEGS_TO_RADIANS = M_PI / 180;
static const double RADIANS_TO_DEGS = 180 / M_PI;

// Only does accel, gyro, and mag
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

} // namespace imu
} // namespace ram


#endif // RAM_IMU_TEST_HELPERS_09_14_2007
