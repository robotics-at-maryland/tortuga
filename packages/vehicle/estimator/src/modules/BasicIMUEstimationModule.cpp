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
#include <log4cpp/Category.hh>

// Project Includes
#include "math/include/Helpers.h"
#include "vehicle/include/Utility.h"
#include "vehicle/estimator/include/modules/BasicIMUEstimationModule.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("StEstIMU"));

namespace ram {
namespace estimator {

BasicIMUEstimationModule::BasicIMUEstimationModule(core::ConfigNode config) :
    m_magIMUName(config["magIMUNum"].asString("MagBoom")),
    m_cgIMUName(config["cgIMUNum"].asString("IMU")),
    m_orientation(math::Quaternion::IDENTITY)
{
    /* initialization from config values should be done here */
    LOGGER.info("% IMU#(0=main,1=boom) Accel Mag Gyro Accel-Raw Mag-Raw"
                " Gyro-Raw Quat TimeStamp");
}

void BasicIMUEstimationModule::init(core::EventPtr event)
{
    // receive the sensor config parameters
    vehicle::IMUInitEventPtr ievent = 
        boost::dynamic_pointer_cast<vehicle::IMUInitEvent>(event);

    if(!event) {
        LOGGER.warn("BasicIMUEstimationModule: init: Invalid Event Type");
    } else {
        LOGGER.info("BasicIMUEstimationModule: init: Config Received " + ievent->name);
        imuList[ievent->name] = ievent;
        m_filteredState[ievent->name] = new FilteredIMUData();
    }
}

void BasicIMUEstimationModule::update(core::EventPtr event, 
                                      EstimatedStatePtr estimatedState)
{

    vehicle::RawIMUDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawIMUDataEvent>(event);

    /* Return if the cast failed and let people know about it. */
    if(!ievent){
        LOGGER.warn("BasicIMUEstimationModule: update: Invalid Event Type");
        return;
    }

    if(imuList.find(ievent->name) == imuList.end()){
        LOGGER.warn("BasicIMUEstimationModule: update: IMU Not Configured");
        return;
    }

    /* This is where the estimation should be done
       The result should be stored in estimatedState */

    std::string name = ievent->name;
    //double timestep = ievent->timestep;
    IMUConfigPtr config = imuList[name];

    RawIMUData newState = ievent->rawIMUData;
    rotateAndFilterData(&newState, name);

    // Use filtered data to get quaternion
    // double linearAcceleration[3] = {m_filteredAccelX[name].getValue(),
    //                                 m_filteredAccelY[name].getValue(),
    //                                 m_filteredAccelZ[name].getValue()};

    // double magnetometer[3] = {m_filteredMagX[name].getValue(),
    //                           m_filteredMagY[name].getValue(),
    //                           m_filteredMagZ[name].getValue()};

    // double quaternion[4] = {0,0,0,1};
    // math::Quaternion updateQuat;
    // {
    //     core::ReadWriteMutex::ScopedWriteLock lock(m_orientationMutex);

	// 	double magLength = math::magnitude3x1(magnetometer);
				
	// 	if(fabs(magLength - config->magNominalLength) < 
    //        config->magCorruptThreshold) {

    //         quaternionFromIMU(magnetometer, linearAcceleration,
    //                           quaternion);

	// 	} else {
    //         double quaternionOld[4] = {m_orientation.x,
    //                                    m_orientation.y,
    //                                    m_orientation.z,
    //                                    m_orientation.w};

    //         double omega[3] = {m_filteredGyroX[name].getValue(),
    //                            m_filteredGyroY[name].getValue(),
    //                            m_filteredGyroZ[name].getValue()};

    //         quaternionFromRate(quaternionOld, omega, timestep,
    //                            quaternion);
	// 	}
		
    //     m_orientation.x = quaternion[0];
    //     m_orientation.y = quaternion[1];
    //     m_orientation.z = quaternion[2];
    //     m_orientation.w = quaternion[3];
    //     updateQuat = m_orientation;
    // }
 
    // OGERize the linear acceleration and magnetometer estimates
    math::Vector3 accel(m_filteredAccelX[m_cgIMUName].getValue(),
                        m_filteredAccelY[m_cgIMUName].getValue(),
                        m_filteredAccelZ[m_cgIMUName].getValue());

    math::Vector3 mag(m_filteredMagX[m_cgIMUName].getValue(),
                      m_filteredMagY[m_cgIMUName].getValue(),
                      m_filteredMagZ[m_cgIMUName].getValue());
    
    
    // if we have the magboom imu, use it for the mag readings
    if(imuList.find(m_magIMUName) != imuList.end()) {
        
        mag[0] = m_filteredMagX[m_magIMUName].getValue();
        mag[1] = m_filteredMagY[m_magIMUName].getValue();
        mag[2] = m_filteredMagZ[m_magIMUName].getValue();

    }

    math::Quaternion estOrientation(vehicle::Utility::quaternionFromMagAccel(mag,accel));
    estimatedState->setEstimatedOrientation(estOrientation);

    // Send Event
    // math::OrientationEventPtr oevent(new math::OrientationEvent());
    // oevent->orientation = updateQuat;
    // publish(IIMU::UPDATE, oevent);

    // // Log data directly
    LOGGER.infoStream() << name << " "
                        << m_filteredAccelX[name].getValue() << " "
                        << m_filteredAccelY[name].getValue() << " "
                        << m_filteredAccelZ[name].getValue() << " "
                        << m_filteredMagX[name].getValue() << " "
                        << m_filteredMagY[name].getValue() << " "
                        << m_filteredMagZ[name].getValue() << " "
                        << m_filteredGyroX[name].getValue() << " "
                        << m_filteredGyroY[name].getValue() << " "
                        << m_filteredGyroZ[name].getValue() << " "
                        << newState.accelX << " "
                        << newState.accelY << " "
                        << newState.accelZ << " "
                        << newState.magX << " " 
                        << newState.magY << " "
                        << newState.magZ << " "
                        << newState.gyroX << " " 
                        << newState.gyroY << " "
                        << newState.gyroZ << " "
                        << estOrientation[0] << " " << estOrientation[1] << " "
                        << estOrientation[2] << " " << estOrientation[3];
}

void BasicIMUEstimationModule::rotateAndFilterData(const RawIMUData* newState, 
                                                   std::string name)
{
    // grab the correct config
    IMUConfigPtr config = imuList[name];

    // Place into arrays
    math::Vector3 linearAcceleration(newState->accelX,
                                     newState->accelY,
                                     newState->accelZ);

    math::Vector3 magnetometer(newState->magX - config->magBias[0],
                               newState->magY - config->magBias[1],
                               newState->magZ - config->magBias[2]);

    math::Vector3 gyro(newState->gyroX - config->gyroBias[0],
                       newState->gyroY - config->gyroBias[1],
                       newState->gyroZ - config->gyroBias[2]);

    // Rotate The Data
    math::Vector3 rotatedLinearAccel = 
        config->IMUtoVehicleFrame * linearAcceleration;

    math::Vector3 rotatedMagnetometer = 
        config->IMUtoVehicleFrame * magnetometer;

    math::Vector3 rotatedGyro = 
        config->IMUtoVehicleFrame * gyro;

    // Filter data
    m_filteredAccelX[name].addValue(rotatedLinearAccel[0]);
    m_filteredAccelY[name].addValue(rotatedLinearAccel[1]);
    m_filteredAccelZ[name].addValue(rotatedLinearAccel[2]);

    // Account for magnetic fields of the frame (ie the thrusters)
    m_filteredMagX[name].addValue(rotatedMagnetometer[0]);
    m_filteredMagY[name].addValue(rotatedMagnetometer[1]);
    m_filteredMagZ[name].addValue(rotatedMagnetometer[2]);

    m_filteredGyroX[name].addValue(rotatedGyro[0]);
    m_filteredGyroY[name].addValue(rotatedGyro[1]);
    m_filteredGyroZ[name].addValue(rotatedGyro[2]);

    // Place filterd state into accel structure
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_filteredState[name]->accelX = m_filteredAccelX[name].getValue();
        m_filteredState[name]->accelY = m_filteredAccelY[name].getValue();
        m_filteredState[name]->accelZ = m_filteredAccelZ[name].getValue();
         
        m_filteredState[name]->magX = m_filteredMagX[name].getValue();
        m_filteredState[name]->magY = m_filteredMagY[name].getValue();
        m_filteredState[name]->magZ = m_filteredMagZ[name].getValue();
         
        m_filteredState[name]->gyroX = m_filteredGyroX[name].getValue();
        m_filteredState[name]->gyroY = m_filteredGyroY[name].getValue();
        m_filteredState[name]->gyroZ = m_filteredGyroZ[name].getValue();
    }
}


void BasicIMUEstimationModule::quaternionFromIMU(double _mag[3], double _accel[3],
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

    math::quaternionFromnCb((double (*)[3])(nCb[0]), quaternion);
}


/*
  an algorithm to estimate the current quaternion given a previous quaternion
  estimate, a current angular rate measurement, and a timestep.  this algorithm
  simply integrates the previous quaternion estimate using angular rate gyro data

  inputs:
  quaternionOld - a double array containing the old quaternion estimate
  angRate - a double array containing the current angular rate gyro data
  deltaT - a double quantifying the timestep from the previous time this function was called
  quaternionNew - pointer to a double array where the new quaternion estimate will be placed

  output: - placed in data location specified by quaternionNew pointer

  note: this routine uses OGRE to simplify the mathematic calculations, but the
  input/output format is designed to work in IMU.cpp which still uses double arrays
*/
void BasicIMUEstimationModule::quaternionFromRate(double* quaternionOld,
                                                  double angRate[3],
                                                  double deltaT,
                                                  double* quaternionNew)
{

	//reformat arguments to OGRE syntax
    math::Quaternion qOld(quaternionOld[0],quaternionOld[1],
                          quaternionOld[2],quaternionOld[3]);

    math::Vector3 omega(angRate[0],angRate[1],angRate[2]);

	//find quaternion derivative based off old quaternion and ang rate
    math::Quaternion qDot = qOld.derivative(omega);

	//trapezoidal integration
    math::Quaternion qNew = qOld + qDot*deltaT;

	//normalize to make qNew a unit quaternion
	qNew.normalise();
	
	//format for output
	quaternionNew[0]=qNew.x;
	quaternionNew[1]=qNew.y;
	quaternionNew[2]=qNew.z;
	quaternionNew[3]=qNew.w;
	
}
} // namespace estimatior
} // namespace ram
