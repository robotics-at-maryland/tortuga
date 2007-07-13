/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/src/device/IMU.cpp
 */

// STD Includes
#include <iostream>
#include <cstdio>

// UNIX Includes
#include <unistd.h>  // for open()

// Project Includes
#include "vehicle/include/device/IMU.h"
#include "math/include/Helpers.h"

#include "imu/include/imuapi.h"
    
namespace ram {
namespace vehicle {
namespace device {

using namespace ram::math;
    
IMU::IMU(Vehicle* vehicle, core::ConfigNode config) :
    Device(vehicle, config["name"].asString()),
    Updatable(),
    Subject(),
    m_devfile(config["devfile"].asString("/dev/imu")),
    m_serialFD(-1),
    m_localMagneticPitch(0),
    m_magXBias(0),
    m_magYBias(0),
    m_magZBias(0),
    m_orientation(0,0,0,1),
    m_rawState(0),
    m_filteredState(0)
{
    m_serialFD = openIMU(m_devfile.c_str());
    m_rawState = new RawIMUData();
    m_filteredState = new FilteredIMUData();

    // Load Rotation Matrix
    m_IMUToVehicleFrame[0][0] =
        config["imuToVehicleRotMatrix"][0][0].asDouble(0);
    m_IMUToVehicleFrame[0][1] =
        config["imuToVehicleRotMatrix"][0][1].asDouble(0);
    m_IMUToVehicleFrame[0][2] =
        config["imuToVehicleRotMatrix"][0][2].asDouble(1);
    
    m_IMUToVehicleFrame[1][0] =
        config["imuToVehicleRotMatrix"][1][0].asDouble(-1);
    m_IMUToVehicleFrame[1][1] =
        config["imuToVehicleRotMatrix"][1][1].asDouble(0);
    m_IMUToVehicleFrame[1][2] =
        config["imuToVehicleRotMatrix"][1][2].asDouble(0);
    
    m_IMUToVehicleFrame[2][0] =
        config["imuToVehicleRotMatrix"][2][0].asDouble(0);
    m_IMUToVehicleFrame[2][1] =
        config["imuToVehicleRotMatrix"][2][1].asDouble(-1);
    m_IMUToVehicleFrame[2][2] =
        config["imuToVehicleRotMatrix"][2][2].asDouble(0);

    m_localMagneticPitch = config["localMagneticPitch"].asDouble();

    m_magXBias = config["magXBias"].asDouble();
    m_magYBias = config["magYBias"].asDouble();
    m_magZBias = config["magZBias"].asDouble();

    //    printf("Bias X: %7.5f Bias Y: %7.5f Bias Z: %7.5f\n", m_magXBias, 
    //	   m_magYBias, m_magZBias);
}

IMU::~IMU()
{
    // Always make sure to shutdwon the background thread
    Updatable::unbackground(true);

    // Only close file if its a non-negative number
    if (m_serialFD >= 0)
        close(m_serialFD);

    delete m_rawState;
    delete m_filteredState;
}

IMUPtr IMU::construct(Vehicle* vehicle, core::ConfigNode config)
{
    return IMUPtr(new IMU(vehicle, config));
}

IMUPtr IMU::castTo(IDevicePtr ptr)
{
    return boost::dynamic_pointer_cast<IMU>(ptr);
}
/*
IMU* IMU::castTo(IDevice* ptr)
{
    return dynamic_cast<IMU*>(ptr);
}
*/ 
void IMU::update(double timestep)
{
//    std::cout << "IMU update" << std::endl;
    // Only grab data on valid fd
    if (m_serialFD >= 0)
    {
        // Grab latest state from vehicle
        RawIMUData newState;
        if (readIMUData(m_serialFD, &newState))
        {
            {
                // Thread safe copy of good imu data
                core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
                *m_rawState = newState;
            }
            
//            printf("MB: %7.4f %7.4f %7.4f ", newState.magX, newState.magY,
//                   newState.magZ);

	    // Account for magnetic fields of the frame (ie the thrusters)
	    newState.magX = newState.magX - m_magXBias;
            newState.magY = newState.magY - m_magYBias;
            newState.magZ = newState.magZ - m_magZBias;
            
	    /*	    printf("IMU F. Bias Raw: %7.4f %7.4f %7.4f\n", newState.magX, 
		   newState.magY,
                   newState.magZ);*/

            
            
            // Rotate into vehicle frame and filter data
            rotateAndFilterData(&newState);

            // Use filtered data to get quaternion
            double linearAcceleration[3] = {0,0,0};
            double magnetometer[3] = {0,0,0};
            
            linearAcceleration[0] = m_filteredAccelX.getValue();
            linearAcceleration[1] = m_filteredAccelY.getValue();
            linearAcceleration[2] = m_filteredAccelZ.getValue();
            
            magnetometer[0] = m_filteredMagX.getValue();
            magnetometer[1] = m_filteredMagY.getValue();
            magnetometer[2] = m_filteredMagZ.getValue();

//            printf(" MF: %7.4f %7.4f %7.4f \n", magnetometer[0],
//                   magnetometer[1], magnetometer[2]);

            {
                double quaternion[4] = {0,0,0,1};
                core::ReadWriteMutex::ScopedWriteLock lock(m_orientationMutex);
                
                quaternionFromIMU(magnetometer, linearAcceleration, quaternion);

                m_orientation.q1 = quaternion[0];
                m_orientation.q2 = quaternion[1];
                m_orientation.q3 = quaternion[2];
                m_orientation.q4 = quaternion[3];

                //printf("Q: %7.4f %7.4f %7.4f %7.4f\n", m_orientation.q1,
                //       m_orientation.q2, m_orientation.q3,
                //       m_orientation.q4);
            }



            // Nofity observers
            setChanged();
            notifyObservers(0, DataUpdate);
        }
    }
    // We didn't connect, try to reconnect
    else
    {
//        m_serialFD = openIMU(m_devfile.c_str());
    }
}
    
math::Vector3 IMU::getLinearAcceleration()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return math::Vector3(m_rawState->accelX, m_rawState->accelY,
                         m_rawState->accelZ);
}

math::Vector3 IMU::getAngularRate()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return math::Vector3(m_rawState->gyroX, m_rawState->gyroY,
                         m_rawState->gyroZ);
}

math::Quaternion IMU::getOrientation()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_orientationMutex);
    return m_orientation;
}
    
void IMU::getRawState(RawIMUData& imuState)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    imuState = *m_rawState;
}

void IMU::getFilteredState(FilteredIMUData& filteredState)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    filteredState= *m_filteredState;
}

void IMU::rotateAndFilterData(RawIMUData* newState)
{
    // Place into arrays
    double linearAcceleration[3];
    double magnetometer[3];
    double gyro[3];
    
    linearAcceleration[0] = newState->accelX;
    linearAcceleration[1] = newState->accelY;
    linearAcceleration[2] = newState->accelZ;

    magnetometer[0] = newState->magX;
    magnetometer[1] = newState->magY;
    magnetometer[2] = newState->magZ;

    gyro[0] = newState->gyroX;
    gyro[1] = newState->gyroY;
    gyro[2] = newState->gyroZ;

    // Rotate The Data
    double rotatedLinearAcceleration[3];
    double rotatedMagnetometer[3];
    double rotatedGyro[3];

    math::matrixMult3x1by3x3(m_IMUToVehicleFrame, linearAcceleration,
                             rotatedLinearAcceleration);

    math::matrixMult3x1by3x3(m_IMUToVehicleFrame, magnetometer,
                             rotatedMagnetometer);
    
    math::matrixMult3x1by3x3(m_IMUToVehicleFrame, gyro,
                             rotatedGyro);

    //    printf("MR: %7.4f %7.4f %7.4f\n", rotatedMagnetometer[0],
    //           rotatedMagnetometer[1], rotatedMagnetometer[2]);
    
    // Filter data
    m_filteredAccelX.addValue(rotatedLinearAcceleration[0]);
    m_filteredAccelY.addValue(rotatedLinearAcceleration[1]);
    m_filteredAccelZ.addValue(rotatedLinearAcceleration[2]);

    m_filteredMagX.addValue(rotatedMagnetometer[0]);
    m_filteredMagY.addValue(rotatedMagnetometer[1]);
    m_filteredMagZ.addValue(rotatedMagnetometer[2]);

    m_filteredGyroX.addValue(rotatedGyro[0]);
    m_filteredGyroY.addValue(rotatedGyro[1]);
    m_filteredGyroZ.addValue(rotatedGyro[2]);

    // Place filterd state into accel structure
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_filteredState->accelX = m_filteredAccelX.getValue();
        m_filteredState->accelY = m_filteredAccelY.getValue();
        m_filteredState->accelZ = m_filteredAccelZ.getValue();
        
        m_filteredState->magX = m_filteredMagX.getValue();
        m_filteredState->magY = m_filteredMagY.getValue();
        m_filteredState->magZ = m_filteredMagZ.getValue();
        
        m_filteredState->gyroX= m_filteredGyroX.getValue();
        m_filteredState->gyroX= m_filteredGyroY.getValue();
        m_filteredState->gyroX = m_filteredGyroZ.getValue();
    }
}

void IMU::quaternionFromIMU(double mag[3], double accel[3], double *quaternion)
{
    double xComponent[3];//top row of DCM
    double yComponent[3];//middle row of DCM
    double zComponent[3];//bottom row of DCM
    double temp[3];
    double rotationMatrix[3][3];//used to account for magnetic inclination
    double DCM[3][3];//direction cosine matrix

    //set xComponent equal to the normalized magnetometer reading
    //load the values
    xComponent[0] = mag[0];
    xComponent[1] = mag[1];
    xComponent[2] = mag[2];
    //normalize
    normalize3x1(xComponent);

    //find yComponent
    //first normalize accel
    temp[0] = accel[0];
    temp[1] = accel[1];
    temp[2] = accel[2];
    normalize3x1(temp);
    //cross product m x g
    crossProduct3x1by3x1(xComponent, temp, yComponent);
    //normalize the yComponent
    normalize3x1(yComponent);

    //find zComponent
    crossProduct3x1by3x1(xComponent, yComponent, zComponent);
    //normalize just for shits and giggles
    normalize3x1(zComponent);

    //place x, y, and z components in the direction cosine matrix
    DCM[0][0] = xComponent[0];
    DCM[0][1] = xComponent[1];
    DCM[0][2] = xComponent[2];
    DCM[1][0] = yComponent[0];
    DCM[1][1] = yComponent[1];
    DCM[1][2] = yComponent[2];
    DCM[2][0] = zComponent[0];
    DCM[2][1] = zComponent[1];
    DCM[2][2] = zComponent[2];

    //find rotation matrix to pitch axes to account for magnetic inclination
    rotationPitch(m_localMagneticPitch, &rotationMatrix[0][0]);

    //rotate DCM
    double DCMrotated[3][3];
    matrixMult3x3by3x3(rotationMatrix, DCM, &DCMrotated[0][0]);

    //extract quaternion from DCM
    quaternionFromDCM(DCMrotated, &quaternion[0]);
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
