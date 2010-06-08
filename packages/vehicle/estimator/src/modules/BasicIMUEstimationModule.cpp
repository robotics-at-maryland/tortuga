/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/src/modules/BasicIMUEstimationModule.cpp
 */

// Library Includes
#include <iostream>
#include <boost/smart_cast.hpp>

// Project Includes
#include "vehicle/include/Events.h"
#include "vehicle/estimator/include/modules/BasicIMUEstimationModule.h"

namespace ram {
namespace estimator {

BasicIMUEstimationModule::BasicIMUEstimationModule(core::ConfigNode config)
{
    /* initialization from config values should be done here */
}

void BasicIMUEstimationModule::init(core::EventPtr event)
{

}

void BasicIMUEstimationModule::update(core::EventPtr event, 
                                      EstimatedStatePtr estimatedState)
{

    vehicle::RawIMUDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawIMUDataEvent>(event);

    /* Return if the cast failed and let people know about it. */
    if(!ievent){
        std::cerr << "BasicIMUEstimationModule: update: Invalid Event Type" 
                  << std::endl;
        return;
    }

    /* This is where the estimation should be done
       The result should be stored in estimatedState */

//     rotateAndFilterData(&newState);

//     // Use filtered data to get quaternion
//     double linearAcceleration[3] = {0,0,0};
//     double magnetometer[3] = {0,0,0};
            
//     linearAcceleration[0] = m_filteredAccelX.getValue();
//     linearAcceleration[1] = m_filteredAccelY.getValue();
//     linearAcceleration[2] = m_filteredAccelZ.getValue();
//     Vector3 linAccel(m_filteredAccelX.getValue(),
//                      m_filteredAccelY.getValue(),
//                      m_filteredAccelZ.getValue()); 
            
//     magnetometer[0] = m_filteredMagX.getValue();
//     magnetometer[1] = m_filteredMagY.getValue();
//     magnetometer[2] = m_filteredMagZ.getValue();
//     Vector3 mag(m_filteredMagX.getValue(),
//                 m_filteredMagY.getValue(),
//                 m_filteredMagZ.getValue());

//     Vector3 angRate(m_filteredGyroX.getValue(),
//                     m_filteredGyroY.getValue(),
//                     m_filteredGyroZ.getValue());
// //            printf(" MF: %7.4f %7.4f %7.4f \n", magnetometer[0],
// //                   magnetometer[1], magnetometer[2]);

//     double quaternion[4] = {0,0,0,1};
//     math::Quaternion updateQuat;
//     {
//         core::ReadWriteMutex::ScopedWriteLock lock(m_orientationMutex);

// 		//m_orientation = computeQuaternion(mag, linAccel, angRate,
// 		//				  timestep, m_orientation);
                
// 		double magLength;
// 		magLength = magnitude3x1(magnetometer);
// 		double difference;
// 		difference = magLength-m_magNominalLength;
// 		difference = fabs(difference);
				
// 		if(difference < m_magCorruptThresh){
//             quaternionFromIMU(magnetometer, linearAcceleration,
//                               quaternion);
// 		}else{
//             double quaternionOld[4] = {0,0,0,0};
//             quaternionOld[0] = m_orientation.x;
//             quaternionOld[1] = m_orientation.y;
//             quaternionOld[2] = m_orientation.z;
//             quaternionOld[3] = m_orientation.w;
//             double omega[3] = {0,0,0};
//             omega[0] = m_filteredGyroX.getValue();
//             omega[1] = m_filteredGyroY.getValue();
//             omega[2] = m_filteredGyroZ.getValue();
//             quaternionFromRate(quaternionOld, omega, timestep,
//                                quaternion);
// 		}

				
				
//         m_orientation.x = quaternion[0];
//         m_orientation.y = quaternion[1];
//         m_orientation.z = quaternion[2];
//         m_orientation.w = quaternion[3];
//         updateQuat = m_orientation;
// //                printf("Q: %7.4f %7.4f %7.4f %7.4f\n", m_orientation.x,
// //                       m_orientation.y, m_orientation.z,
// //                       m_orientation.w);
//     }

//     // Send Event
//     math::OrientationEventPtr oevent(new math::OrientationEvent());
//     oevent->orientation = updateQuat;
//     publish(IIMU::UPDATE, oevent);

//     // Log data directly
//     LOGGER.infoStream() << m_imuNum << " "
//                         << m_filteredAccelX.getValue() << " "
//                         << m_filteredAccelY.getValue() << " "
//                         << m_filteredAccelZ.getValue() << " "
//                         << m_filteredMagX.getValue() << " "
//                         << m_filteredMagY.getValue() << " "
//                         << m_filteredMagZ.getValue() << " "
//                         << m_filteredGyroX.getValue() << " "
//                         << m_filteredGyroY.getValue() << " "
//                         << m_filteredGyroZ.getValue() << " "
//                         << newState.accelX << " "
//                         << newState.accelY << " "
//                         << newState.accelZ << " "
//                         << newState.magX << " " 
//                         << newState.magY << " "
//                         << newState.magZ << " "
//                         << newState.gyroX << " " 
//                         << newState.gyroY << " "
//                         << newState.gyroZ << " "
//                         << quaternion[0] << " " << quaternion[1] << " "
//                         << quaternion[2] << " " << quaternion[3];
// }
// }
// // We didn't connect, try to reconnect
// else
// {
// //        m_serialFD = openIMU(m_devfile.c_str());
// }
// }

// void BasicIMUEstimationModule::rotateAndFilterData(const RawIMUData* newState)
// {
//     // Place into arrays
//     double linearAcceleration[3];
//     double magnetometer[3];
//     double gyro[3];
    
//     linearAcceleration[0] = newState->accelX;
//     linearAcceleration[1] = newState->accelY;
//     linearAcceleration[2] = newState->accelZ;

//     magnetometer[0] = newState->magX - m_magXBias;
//     magnetometer[1] = newState->magY - m_magYBias;
//     magnetometer[2] = newState->magZ - m_magZBias;

//     gyro[0] = newState->gyroX - m_gyroXBias;
//     gyro[1] = newState->gyroY - m_gyroYBias;
//     gyro[2] = newState->gyroZ - m_gyroZBias;

//     // Rotate The Data
//     double rotatedLinearAcceleration[3];
//     double rotatedMagnetometer[3];
//     double rotatedGyro[3];

//     math::matrixMult3x1by3x3(m_IMUToVehicleFrame, linearAcceleration,
//                              rotatedLinearAcceleration);

//     math::matrixMult3x1by3x3(m_IMUToVehicleFrame, magnetometer,
//                              rotatedMagnetometer);
    
//     math::matrixMult3x1by3x3(m_IMUToVehicleFrame, gyro,
//                              rotatedGyro);

//     //    printf("MR: %7.4f %7.4f %7.4f\n", rotatedMagnetometer[0],
//     //           rotatedMagnetometer[1], rotatedMagnetometer[2]);
    
//     // Filter data
//     m_filteredAccelX.addValue(rotatedLinearAcceleration[0]);
//     m_filteredAccelY.addValue(rotatedLinearAcceleration[1]);
//     m_filteredAccelZ.addValue(rotatedLinearAcceleration[2]);

//     // Account for magnetic fields of the frame (ie the thrusters)
//     m_filteredMagX.addValue(rotatedMagnetometer[0]);
//     m_filteredMagY.addValue(rotatedMagnetometer[1]);
//     m_filteredMagZ.addValue(rotatedMagnetometer[2]);

//     m_filteredGyroX.addValue(rotatedGyro[0]);
//     m_filteredGyroY.addValue(rotatedGyro[1]);
//     m_filteredGyroZ.addValue(rotatedGyro[2]);

//     // Place filterd state into accel structure
//     {
//         core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
//         m_filteredState->accelX = m_filteredAccelX.getValue();
//         m_filteredState->accelY = m_filteredAccelY.getValue();
//         m_filteredState->accelZ = m_filteredAccelZ.getValue();
        
//         m_filteredState->magX = m_filteredMagX.getValue();
//         m_filteredState->magY = m_filteredMagY.getValue();
//         m_filteredState->magZ = m_filteredMagZ.getValue();
        
//         m_filteredState->gyroX= m_filteredGyroX.getValue();
//         m_filteredState->gyroY= m_filteredGyroY.getValue();
//         m_filteredState->gyroZ= m_filteredGyroZ.getValue();
//     }
// }


// void IMU::quaternionFromIMU(double _mag[3], double _accel[3],
//                             double *quaternion)
// {
//     Vector3 mag(_mag);
//     Vector3 accel(_accel);
    
//     mag.normalise();
//     Vector3 n3 = accel * -1;
//     n3.normalise();
    
//     Vector3 n2 = mag.crossProduct(accel);
//     n2.normalise();
    
//     Vector3 n1 = n2.crossProduct(n3);
//     n1.normalise();

//     Matrix3 bCn;
//     bCn.SetColumn(0, n1);
//     bCn.SetColumn(1, n2);
//     bCn.SetColumn(2, n3);
    
// 	//legacy version
//     Matrix3 nCb = bCn.Transpose();

//     quaternionFromnCb((double (*)[3])(nCb[0]), quaternion);
// 		/*
// 		but shouldn't it be
// 		quaternionFromnCb((double (*)[3])(bCn[0]), quaternion);
// 		*/

// }


// /*
// an algorithm to estimate the current quaternion given a previous quaternion
// estimate, a current angular rate measurement, and a timestep.  this algorithm
// simply integrates the previous quaternion estimate using angular rate gyro data

// inputs:
// quaternionOld - a double array containing the old quaternion estimate
// angRate - a double array containing the current angular rate gyro data
// deltaT - a double quantifying the timestep from the previous time this function was called
// quaternionNew - pointer to a double array where the new quaternion estimate will be placed

// output: - placed in data location specified by quaternionNew pointer

// note: this routine uses OGRE to simplify the mathematic calculations, but the
// input/output format is designed to work in IMU.cpp which still uses double arrays
// */
// void BasicIMUEstimationModule::quaternionFromRate(double* quaternionOld,
// 							   double angRate[3],
// 							   double deltaT,
// 							   double* quaternionNew){

// 	//reformat arguments to OGRE syntax
// 	Quaternion qOld(quaternionOld[0],quaternionOld[1],
// 					quaternionOld[2],quaternionOld[3]);
// 	Vector3 omega(angRate[0],angRate[1],angRate[2]);

// 	//find quaternion derivative based off old quaternion and ang rate
// 	Quaternion qDot;
// 	qDot = qOld.derivative(omega);

// 	//trapezoidal integration
// 	Quaternion qNew;
// 	qNew = qOld + qDot*deltaT;

// 	//normalize to make qNew a unit quaternion
// 	qNew.normalise();
	
// 	//format for output
// 	quaternionNew[0]=qNew.x;
// 	quaternionNew[1]=qNew.y;
// 	quaternionNew[2]=qNew.z;
// 	quaternionNew[3]=qNew.w;
	
// }

// /*
//  */
// Quaternion BasicIMUEstimationModule::computeQuaternion(Vector3 mag, Vector3 accel,
// 				  Vector3 angRate,
// 				  double deltaT,
// 				  Quaternion quaternionOld){
//   Quaternion dummy(0,0,0,1);
//   double magLength;
//   magLength = mag.length();
//   double difference;
//   difference = magLength-m_magNominalLength;
//   difference = fabs(difference);
				
//   if(difference < m_magCorruptThresh){
//     // should update quaternionFromIMU to take OGRE arguments instead of 
//     // double arrays
//     //    quaternionFromIMU(magnetometer, linearAcceleration, quaternion);
//   }else{
//     double quaternionOld[4] = {0,0,0,0};
//     quaternionOld[0] = m_orientation.x;
//     quaternionOld[1] = m_orientation.y;
//     quaternionOld[2] = m_orientation.z;
//     quaternionOld[3] = m_orientation.w;
//     double omega[3] = {0,0,0};
//     omega[0] = m_filteredGyroX.getValue();
//     omega[1] = m_filteredGyroY.getValue();
//     omega[2] = m_filteredGyroZ.getValue();
//     // should update quaternionFromRate to take OGRE arguments instead of
//     // double arrays
//     //    quaternionFromRate(quaternionOld, angRate, deltaT, dummy);
//   }

//   return dummy;
}

} // namespace estimatior
} // namespace ram
