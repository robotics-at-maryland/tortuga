/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/LoopStateEstimator.cpp
 */

// Library Includes
#include <log4cpp/Category.hh>

// Project Includes
#include "vehicle/include/device/LoopStateEstimator.h"
#include "vehicle/include/device/IIMU.h"
#include "vehicle/include/device/IDVL.h"
#include "vehicle/include/device/IDepthSensor.h"
#include "math/include/Helpers.h"
#include "math/include/Vector3.h"
#include "math/include/Matrix3.h"
#include "math/include/Events.h"

#include "drivers/imu/include/imuapi.h"

static log4cpp::Category& IMU_LOGGER(log4cpp::Category::getInstance("IMU"));

namespace ram {
namespace vehicle {
namespace device {

LoopStateEstimator::LoopStateEstimator(core::ConfigNode config,
                                       core::EventHubPtr eventHub,
                                       IVehiclePtr vehicle) :
    Device(config["name"].asString()),
    IStateEstimator(eventHub, config["name"].asString()),
    m_orientation(math::Quaternion::IDENTITY),
    m_velocity(math::Vector2::ZERO),
    m_position(math::Vector2::ZERO),
    m_depth(0),
    m_filteredState(NULL)
{
    // IMU configuration
    m_filteredState = new FilteredIMUData();

    IMU_LOGGER.info("% IMU#(0=main,1=boom) Accel Mag Gyro Accel-Raw Mag-Raw"
                " Gyro-Raw Quat TimeStamp");


    // DVL configuration values
    // NONE YET!


    // Should loggers go in the state estimator or the device itself?
}
    
LoopStateEstimator::~LoopStateEstimator()
{
    delete m_filteredState;
}

void LoopStateEstimator::imuUpdate(IMUPacket* packet)
{
    // Unpack the raw state
    RawIMUData* rawData = packet->rawData;

    // Read from new state, un-rotate, un-bias and place into 
    // filtered values
    rotateAndFilterData(rawData, packet);

    // Use filtered data to get quaternion
    double linearAcceleration[3] = {0,0,0};
    double magnetometer[3] = {0,0,0};
            
    linearAcceleration[0] = m_imuData.filteredAccelX.getValue();
    linearAcceleration[1] = m_imuData.filteredAccelY.getValue();
    linearAcceleration[2] = m_imuData.filteredAccelZ.getValue();
    math::Vector3 linAccel(m_imuData.filteredAccelX.getValue(),
		     m_imuData.filteredAccelY.getValue(),
		     m_imuData.filteredAccelZ.getValue()); 
            
    magnetometer[0] = m_imuData.filteredMagX.getValue();
    magnetometer[1] = m_imuData.filteredMagY.getValue();
    magnetometer[2] = m_imuData.filteredMagZ.getValue();
    math::Vector3 mag(m_imuData.filteredMagX.getValue(),
		m_imuData.filteredMagY.getValue(),
		m_imuData.filteredMagZ.getValue());

    math::Vector3 angRate(m_imuData.filteredGyroX.getValue(),
		    m_imuData.filteredGyroY.getValue(),
		    m_imuData.filteredGyroZ.getValue());
    //            printf(" MF: %7.4f %7.4f %7.4f \n", magnetometer[0],
    //                   magnetometer[1], magnetometer[2]);

    double quaternion[4] = {0,0,0,1};
    math::Quaternion updateQuat;
    {
	core::ReadWriteMutex::ScopedWriteLock lock(m_orientationMutex);

	//m_orientation = computeQuaternion(mag, linAccel, angRate,
	//				  timestep, m_orientation);
                
	double magLength;
	magLength = math::magnitude3x1(magnetometer);
	double difference;
	difference = magLength-packet->magNominalLength;
	difference = fabs(difference);
				
	if(difference < packet->magCorruptThresh){
	    quaternionFromIMU(magnetometer, linearAcceleration,
			      quaternion);
	} else {
	    double quaternionOld[4] = {0,0,0,0};
	    quaternionOld[0] = m_orientation.x;
	    quaternionOld[1] = m_orientation.y;
	    quaternionOld[2] = m_orientation.z;
	    quaternionOld[3] = m_orientation.w;
	    double omega[3] = {0,0,0};
	    omega[0] = m_imuData.filteredGyroX.getValue();
	    omega[1] = m_imuData.filteredGyroY.getValue();
	    omega[2] = m_imuData.filteredGyroZ.getValue();
	    quaternionFromRate(quaternionOld, omega, packet->timestep,
			       quaternion);
	}
			
	m_orientation.x = quaternion[0];
	m_orientation.y = quaternion[1];
	m_orientation.z = quaternion[2];
	m_orientation.w = quaternion[3];
	updateQuat = m_orientation;
	//                printf("Q: %7.4f %7.4f %7.4f %7.4f\n", m_orientation.x,
	//                       m_orientation.y, m_orientation.z,
	//                       m_orientation.w);
    }


    core::ReadWriteMutex::ScopedWriteLock lock(m_orientationMutex);
    m_orientation = updateQuat;

    // Publish the new orientation
    publishOrientation();

    // Log data directly
    IMU_LOGGER.infoStream() << packet->imuNum << " "
			<< m_imuData.filteredAccelX.getValue() << " "
			<< m_imuData.filteredAccelY.getValue() << " "
			<< m_imuData.filteredAccelZ.getValue() << " "
			<< m_imuData.filteredMagX.getValue() << " "
			<< m_imuData.filteredMagY.getValue() << " "
			<< m_imuData.filteredMagZ.getValue() << " "
			<< m_imuData.filteredGyroX.getValue() << " "
			<< m_imuData.filteredGyroY.getValue() << " "
			<< m_imuData.filteredGyroZ.getValue() << " "
			<< rawData->accelX << " "
			<< rawData->accelY << " "
			<< rawData->accelZ << " "
			<< rawData->magX << " " 
			<< rawData->magY << " "
			<< rawData->magZ << " "
			<< rawData->gyroX << " " 
			<< rawData->gyroY << " "
			<< rawData->gyroZ << " "
			<< quaternion[0] << " " << quaternion[1] << " "
			<< quaternion[2] << " " << quaternion[3];
}

void LoopStateEstimator::dvlUpdate(DVLPacket* rawData)
{
    // Does not support the DVL
    //core::ReadWriteMutex::ScopedWriteLock lock(m_velocityMutex);
    //m_velocity = velocity;
}
    
void LoopStateEstimator::depthUpdate(DepthPacket* rawData)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_depthMutex);
    m_depth = rawData->depth;

    // Publish the event
    publishDepth();
}
    
math::Quaternion LoopStateEstimator::getOrientation(std::string obj)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_orientationMutex);
    return m_orientation;
}

math::Vector3 LoopStateEstimator::getLinearAcceleration()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_imuStateMutex);
    return math::Vector3(m_filteredState->accelX, m_filteredState->accelY,
                         m_filteredState->accelZ);
}

math::Vector3 LoopStateEstimator::getMagnetometer()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_imuStateMutex);
    return math::Vector3(m_filteredState->magX, m_filteredState->magY,
                         m_filteredState->magZ);
}

math::Vector3 LoopStateEstimator::getAngularRate()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_imuStateMutex);
    return math::Vector3(m_filteredState->gyroX, m_filteredState->gyroY,
                         m_filteredState->gyroZ);
}

math::Vector2 LoopStateEstimator::getVelocity(std::string obj)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_velocityMutex);
    return m_velocity;
}

math::Vector2 LoopStateEstimator::getPosition(std::string obj)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_positionMutex);
    return m_position;
}
    
double LoopStateEstimator::getDepth(std::string obj)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_depthMutex);
    return m_depth;
}

bool LoopStateEstimator::hasObject(std::string obj)
{
    return obj == "vehicle";
}

void LoopStateEstimator::getFilteredIMUState(FilteredIMUData& filteredState)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_imuStateMutex);
    filteredState = *m_filteredState;
}

void LoopStateEstimator::rotateAndFilterData(const RawIMUData* newState,
					     IMUPacket* packet)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_imuStateMutex);
    // Place into arrays
    double linearAcceleration[3];
    double magnetometer[3];
    double gyro[3];
	
    linearAcceleration[0] = newState->accelX;
    linearAcceleration[1] = newState->accelY;
    linearAcceleration[2] = newState->accelZ;
	
    magnetometer[0] = newState->magX - packet->magXBias;
    magnetometer[1] = newState->magY - packet->magYBias;
    magnetometer[2] = newState->magZ - packet->magZBias;

    gyro[0] = newState->gyroX - packet->gyroXBias;
    gyro[1] = newState->gyroY - packet->gyroYBias;
    gyro[2] = newState->gyroZ - packet->gyroZBias;

    // Rotate The Data
    double rotatedLinearAcceleration[3];
    double rotatedMagnetometer[3];
    double rotatedGyro[3];

    math::matrixMult3x1by3x3(packet->IMUToVehicleFrame, linearAcceleration,
			     rotatedLinearAcceleration);

    math::matrixMult3x1by3x3(packet->IMUToVehicleFrame, magnetometer,
			     rotatedMagnetometer);
    
    math::matrixMult3x1by3x3(packet->IMUToVehicleFrame, gyro,
			     rotatedGyro);

    //    printf("MR: %7.4f %7.4f %7.4f\n", rotatedMagnetometer[0],
    //           rotatedMagnetometer[1], rotatedMagnetometer[2]);
    
    // Filter data
    m_imuData.filteredAccelX.addValue(rotatedLinearAcceleration[0]);
    m_imuData.filteredAccelY.addValue(rotatedLinearAcceleration[1]);
    m_imuData.filteredAccelZ.addValue(rotatedLinearAcceleration[2]);

    // Account for magnetic fields of the frame (ie the thrusters)
    m_imuData.filteredMagX.addValue(rotatedMagnetometer[0]);
    m_imuData.filteredMagY.addValue(rotatedMagnetometer[1]);
    m_imuData.filteredMagZ.addValue(rotatedMagnetometer[2]);

    m_imuData.filteredGyroX.addValue(rotatedGyro[0]);
    m_imuData.filteredGyroY.addValue(rotatedGyro[1]);
    m_imuData.filteredGyroZ.addValue(rotatedGyro[2]);

    // Place filtered state into accel structure
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_imuStateMutex);
        m_filteredState->accelX = m_imuData.filteredAccelX.getValue();
        m_filteredState->accelY = m_imuData.filteredAccelY.getValue();
        m_filteredState->accelZ = m_imuData.filteredAccelZ.getValue();
        
        m_filteredState->magX = m_imuData.filteredMagX.getValue();
        m_filteredState->magY = m_imuData.filteredMagY.getValue();
        m_filteredState->magZ = m_imuData.filteredMagZ.getValue();
        
        m_filteredState->gyroX= m_imuData.filteredGyroX.getValue();
        m_filteredState->gyroY= m_imuData.filteredGyroY.getValue();
        m_filteredState->gyroZ= m_imuData.filteredGyroZ.getValue();
    }
}

void LoopStateEstimator::quaternionFromIMU(double _mag[3], double _accel[3],
					   double *quaternion)
{
    math::Vector3 mag(_mag);
    math::Vector3 accel(_accel);
    
    mag.normalise();
    math::Vector3 n3 = accel * -1;
    n3.normalise();
    
    math::Vector3 n2 = mag.crossProduct(accel);
    n2.normalise();
    
    math::Vector3 n1 = n2.crossProduct(n3);
    n1.normalise();

    math::Matrix3 bCn;
    bCn.SetColumn(0, n1);
    bCn.SetColumn(1, n2);
    bCn.SetColumn(2, n3);
    
	//legacy version
    math::Matrix3 nCb = bCn.Transpose();

    using namespace math;
    quaternionFromnCb((double (*)[3])(nCb[0]), quaternion);
		/*
		but shouldn't it be
		quaternionFromnCb((double (*)[3])(bCn[0]), quaternion);
		*/
}

void LoopStateEstimator::quaternionFromRate(double* quaternionOld,
					    double angRate[3],
					    double deltaT,
					    double* quaternionNew)
{
    //reformat arguments to OGRE syntax
    math::Quaternion qOld(quaternionOld[0],quaternionOld[1],
		    quaternionOld[2],quaternionOld[3]);
    math::Vector3 omega(angRate[0],angRate[1],angRate[2]);

    //find quaternion derivative based off old quaternion and ang rate
    math::Quaternion qDot;
    qDot = qOld.derivative(omega);

    //trapezoidal integration
    math::Quaternion qNew;
    qNew = qOld + qDot*deltaT;

    //normalize to make qNew a unit quaternion
    qNew.normalise();
	
    //format for output
    quaternionNew[0]=qNew.x;
    quaternionNew[1]=qNew.y;
    quaternionNew[2]=qNew.z;
    quaternionNew[3]=qNew.w;
}

math::Quaternion LoopStateEstimator::computeQuaternion(math::Vector3 mag,
    math::Vector3 accel, math::Vector3 angRate, double deltaT,
    math::Quaternion quaternionOld, const IMUPacket* packet)
{
    math::Quaternion dummy(0,0,0,1);
    double magLength;
    magLength = mag.length();
    double difference;
    difference = magLength-packet->magNominalLength;
    difference = fabs(difference);
				
    if (difference < packet->magCorruptThresh) {
	// should update quaternionFromIMU to take OGRE arguments instead of 
	// double arrays
	//    quaternionFromIMU(magnetometer, linearAcceleration, quaternion);
    } else {
	double quaternionOld[4] = {0,0,0,0};
	quaternionOld[0] = m_orientation.x;
	quaternionOld[1] = m_orientation.y;
	quaternionOld[2] = m_orientation.z;
	quaternionOld[3] = m_orientation.w;
	double omega[3] = {0,0,0};
	omega[0] = m_imuData.filteredGyroX.getValue();
	omega[1] = m_imuData.filteredGyroY.getValue();
	omega[2] = m_imuData.filteredGyroZ.getValue();
	// should update quaternionFromRate to take OGRE arguments instead of
	// double arrays
	//    quaternionFromRate(quaternionOld, angRate, deltaT, dummy);
    }

    return dummy;
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
