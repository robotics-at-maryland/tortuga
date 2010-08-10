/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/src/BWPDController.cpp
 */

// STD Includes
#include <cmath>
#include <cstdio>
#include <iostream>

#ifdef RAM_WINDOWS
#define M_PI 3.14159265358979323846
#endif

// Library Includes
#include <log4cpp/Category.hh>

// Project Includes
#include "control/include/BWPDController.h"
#include "control/include/ControlFunctions.h"

#include "vehicle/include/IVehicle.h"

#include "core/include/SubsystemMaker.h"
#include "core/include/EventHub.h"
#include "core/include/TimeVal.h"

#include "math/include/Helpers.h"
#include "math/include/Vector3.h"
#include "math/include/Events.h"
//#include "imu/include/imuapi.h"

// Register controller in subsystem maker system
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::control::bwpd::BWPDController, BWPDController);

// Create category for logging
static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("Controller"));
//static log4cpp::Category& ADPT_LOGGER(log4cpp::Category::getInstance("AdaptCtrl"));

using namespace std;

namespace ram {
namespace control {
namespace bwpd {

BWPDController::BWPDController(vehicle::IVehiclePtr vehicle,
                               estimation::IStateEstimatorPtr estimator,
                               core::ConfigNode config) :
    IController(config["name"].asString()),
    m_atDepth(false),
    m_atOrientation(false),
    m_depthThreshold(0),
    m_orientationThreshold(0),
    m_vehicle(vehicle),
    m_estimator(estimator),
    m_config(config),
    m_desiredState(0),
    m_measuredState(0),
    m_estimatedState(0),
    m_controllerState(0),
    m_buoyantTorqueCorrection(0, 0, 0)
{   
    init(config); 
}

BWPDController::BWPDController(core::ConfigNode config,
                               core::SubsystemList deps) :
    IController(config["name"].asString(),
                core::Subsystem::getSubsystemOfType<core::EventHub>(deps)),
    m_atDepth(false),
    m_atOrientation(false),
    m_depthThreshold(0),
    m_orientationThreshold(0),
    m_vehicle(core::Subsystem::getSubsystemOfType<vehicle::IVehicle>(deps)),
    m_estimator(core::Subsystem::getSubsystemOfType<estimation::IStateEstimator>(deps)),
    m_config(config),
    m_desiredState(0),
    m_measuredState(0),
    m_estimatedState(0),
    m_controllerState(0)
{
    init(config);
}

BWPDController::~BWPDController()
{
    unbackground(true);
	
    delete m_desiredState;
    delete m_measuredState;
    delete m_controllerState;
    delete m_estimatedState;
}

void BWPDController::setVelocity(math::Vector2 velocity)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_desiredEstimatedStateMutex);
    m_desiredState->velocity = velocity;
    m_controllerState->useVelocityControl = true;
}

math::Vector2 BWPDController::getVelocity()
{
    return math::Vector2::ZERO;
}
   
void BWPDController::setSpeed(double speed)
{
    // Clamp speed
    if (speed > 5)
        speed = 5;
    else if (speed < -5)
        speed = -5;
    
    core::ReadWriteMutex::ScopedWriteLock lock(m_desiredEstimatedStateMutex);
    m_desiredState->speed = speed;
    m_controllerState->useVelocityControl = false;
}

void BWPDController::setSidewaysSpeed(double speed)
{
    // Clamp speed
    if (speed > 5)
        speed = 5;
    else if (speed < -5)
        speed = -5;
    
    core::ReadWriteMutex::ScopedWriteLock lock(m_desiredEstimatedStateMutex);
    m_desiredState->sidewaysSpeed = speed;
    m_controllerState->useVelocityControl = false;
}

void BWPDController::setHeading(double degrees)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_desiredEstimatedStateMutex);

    // Put math here
}

void BWPDController::setDepth(double depth)
{

    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_desiredEstimatedStateMutex);
        if (depth < 0)
            depth = 0;
        m_desiredState->depth = depth;
    }
    
    math::NumericEventPtr event(new math::NumericEvent());
    event->number = depth;
    publish(IController::DESIRED_DEPTH_UPDATE, event);

    // Make sure to publish if we set a depth that is within our range
    if (atDepth())
        publishAtDepth();
}

double BWPDController::getSpeed()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_desiredEstimatedStateMutex);
    return m_desiredState->speed;
}

double BWPDController::getSidewaysSpeed()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_desiredEstimatedStateMutex);
    return m_desiredState->sidewaysSpeed;
}

double BWPDController::getHeading()
{
    //core::ReadWriteMutex::ScopedReadLock lock(m_desiredEstimatedStateMutex);
    return 0;
}

double BWPDController::getDepth()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_desiredEstimatedStateMutex);
    return m_desiredState->depth;
}



double BWPDController::getEstimatedDepth()
{
    // BROKEN!!!!
    core::ReadWriteMutex::ScopedReadLock lock(m_desiredEstimatedStateMutex);
    assert(false && "Not yet implemented");
    //return appropriate xHat value here
    return 0;
}

double BWPDController::getEstimatedDepthDot()
{
    // BROKEN!!!!!
    core::ReadWriteMutex::ScopedReadLock lock(m_desiredEstimatedStateMutex);
    assert(false && "Not yet implemented");
    //return appropriate xHat value here
    return 0;
}
    
void BWPDController::rollVehicle(double degrees)
{
  //use Helpers.cpp
  using namespace ram::math;

  //fetch old desired quaternion
  core::ReadWriteMutex::ScopedWriteLock lock(m_desiredEstimatedStateMutex);

  //create rotation quaternion based on user input
  double rotationQuaternion[4];
  double rollAxis[3] = {1,0,0};
  quaternionFromEulerAxis(rollAxis,degrees*M_PI/180,rotationQuaternion);
  
  //rotate the quaternion and store in a new place
  double newQuaternion[4];
  quaternionCrossProduct(m_desiredState->quaternion, rotationQuaternion,
			 newQuaternion);

  //store the new quaternion as the new desired quaternion
  m_desiredState->quaternion[0] = newQuaternion[0];
  m_desiredState->quaternion[1] = newQuaternion[1];
  m_desiredState->quaternion[2] = newQuaternion[2];
  m_desiredState->quaternion[3] = newQuaternion[3];

  math::OrientationEventPtr event(new math::OrientationEvent());
  event->orientation = math::Quaternion(m_desiredState->quaternion);
  publish(IController::DESIRED_ORIENTATION_UPDATE, event);
}

void BWPDController::pitchVehicle(double degrees)
{
  //use Helpers.cpp
  using namespace ram::math;

  //fetch old desired quaternion
  core::ReadWriteMutex::ScopedWriteLock lock(m_desiredEstimatedStateMutex);

  //create rotation quaternion based on user input
  double rotationQuaternion[4];
  double rollAxis[3] = {0,1,0};
  quaternionFromEulerAxis(rollAxis,degrees*M_PI/180,rotationQuaternion);
  
  //rotate the quaternion and store in a new place
  double newQuaternion[4];
  quaternionCrossProduct(m_desiredState->quaternion, rotationQuaternion,
			 newQuaternion);

  //store the new quaternion as the new desired quaternion
  m_desiredState->quaternion[0] = newQuaternion[0];
  m_desiredState->quaternion[1] = newQuaternion[1];
  m_desiredState->quaternion[2] = newQuaternion[2];
  m_desiredState->quaternion[3] = newQuaternion[3];

  math::OrientationEventPtr event(new math::OrientationEvent());
  event->orientation = math::Quaternion(m_desiredState->quaternion);
  publish(IController::DESIRED_ORIENTATION_UPDATE, event);
}

void BWPDController::yawVehicle(double degrees)
{
  //use Helpers.cpp
  using namespace ram::math;

  //fetch old desired quaternion
  core::ReadWriteMutex::ScopedWriteLock lock(m_desiredEstimatedStateMutex);

  //create rotation quaternion based on user input
  double rotationQuaternion[4];
  double rollAxis[3] = {0,0,1};
  quaternionFromEulerAxis(rollAxis,degrees*M_PI/180,rotationQuaternion);
  
  //rotate the quaternion and store in a new place
  double newQuaternion[4];
  quaternionCrossProduct(m_desiredState->quaternion, rotationQuaternion,
			 newQuaternion);

  //store the new quaternion as the new desired quaternion
  m_desiredState->quaternion[0] = newQuaternion[0];
  m_desiredState->quaternion[1] = newQuaternion[1];
  m_desiredState->quaternion[2] = newQuaternion[2];
  m_desiredState->quaternion[3] = newQuaternion[3];

  math::OrientationEventPtr event(new math::OrientationEvent());
  event->orientation = math::Quaternion(m_desiredState->quaternion);
   publish(IController::DESIRED_ORIENTATION_UPDATE, event);


  if(atOrientation())
  {
      publishAtOrientation();
  }
}

math::Quaternion BWPDController::getDesiredOrientation()
{
    return math::Quaternion(m_desiredState->quaternion);
}

void BWPDController::setDesiredOrientation(math::Quaternion newQuaternion)
{
	//fetch old desired quaternion
	core::ReadWriteMutex::ScopedWriteLock lock(m_desiredEstimatedStateMutex);
	
	//we do not want to apply a non-normalized quaternion; the 
	//non-normalization could propagate through internal controller variables
	newQuaternion.normalise();
	
	//store the new quaternion as the new desired quaternion
	m_desiredState->quaternion[0] = newQuaternion.x;
	m_desiredState->quaternion[1] = newQuaternion.y;
	m_desiredState->quaternion[2] = newQuaternion.z;
	m_desiredState->quaternion[3] = newQuaternion.w;
	
	math::OrientationEventPtr event(new math::OrientationEvent());
	event->orientation = math::Quaternion(m_desiredState->quaternion);
	publish(IController::DESIRED_ORIENTATION_UPDATE, event);

    if(atOrientation())
    {
        publishAtOrientation();
    }
}
    
bool BWPDController::atOrientation()
{
    return doIsOriented(m_measuredState, m_desiredState,
                        m_orientationThreshold);
}

bool BWPDController::atDepth()
{
    double difference = fabs(m_measuredState->depth - m_desiredState->depth);
    return difference <= m_depthThreshold;
}

void BWPDController::holdCurrentDepth()
{
    // Set depth to the current vehicle depth
    setDepth(m_estimator->getEstimatedDepth());
}
    
  /*
used to load the current heading into desiredState
desiredState->quaternion will be "level" in horizontal plane
the vehicle should be upright when using this function, otherwise
OGRE's interpretation of yaw and upright will be nonsensical
   */

void BWPDController::holdCurrentHeading()
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_desiredEstimatedStateMutex);
    //get current orientation
    //math::Quaternion q(m_measuredState->quaternion);
    math::Quaternion qCurrent(m_estimator->getEstimatedOrientation());

    //find approximation of vehicle yaw based off orientation
    math::Radian yaw = qCurrent.getYaw();

    //compute "level" quaternion based off yaw value
    math::Quaternion q(math::Radian(yaw),math::Vector3::UNIT_Z);

    //save to desiredState
    m_desiredState->quaternion[0] = q.x;
    m_desiredState->quaternion[1] = q.y;
    m_desiredState->quaternion[2] = q.z;
    m_desiredState->quaternion[3] = q.w;


    //we want to hold a desired position, so we'll want angular rate to be zero
    m_desiredState->angularRate[0] = 0;
    m_desiredState->angularRate[1] = 0;
    m_desiredState->angularRate[2] = 0;

    math::OrientationEventPtr event(new math::OrientationEvent());
    event->orientation = math::Quaternion(m_desiredState->quaternion);
    publish(IController::DESIRED_ORIENTATION_UPDATE, event);
}

void BWPDController::holdCurrentPosition()
{
    // TODO: Place code here
}

void BWPDController::setBuoyantTorqueCorrection(double x, double y, double z)
{
    math::Vector3 temp(x, y, z);
    m_buoyantTorqueCorrection = temp;
}

void BWPDController::update(double timestep)
{
    if (m_initializationPause > 0)
    {
        m_initializationPause -= timestep;
    	return;
    }
    else
    {
        if (m_holdStartOrientation)
        {
            holdCurrentHeading();
            m_holdStartOrientation = false;
        }
    }

    // Grab latest state (preform small hack to copy it over for the controller)
    math::Vector3 linearAcceleration(m_estimator->getEstimatedLinearAcceleration());
    memcpy(&m_measuredState->linearAcceleration[0],
           &linearAcceleration, sizeof(double) * 3);

    //std::cout << "A: " << linearAcceleration << std::endl;
    math::Quaternion orientation(m_estimator->getEstimatedOrientation());
    memcpy(&m_measuredState->quaternion[0],
           &orientation, sizeof(double) * 4);

    //std::cout << "O: " << orientation << std::endl;
    
    math::Vector3 angularRate(m_estimator->getEstimatedAngularRate());
    memcpy(&m_measuredState->angularRate[0],
           &angularRate, sizeof(double) * 3);

    //std::cout << "W: " << angularRate << std::endl;
    m_measuredState->depth = m_estimator->getEstimatedDepth();

    m_measuredState->velocity = m_estimator->getEstimatedVelocity();
    
    // Calculate new forces
    math::Vector3 translationalForce(0,0,0);
    math::Vector3 rotationalTorque(0,0,0);

    {
        core::ReadWriteMutex::ScopedReadLock lock(m_desiredEstimatedStateMutex);
        
        translationalController(m_measuredState, m_desiredState,
        	                m_controllerState, m_estimatedState,
				timestep, translationalForce.ptr());

	/*
	  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  MAKE SURE THAT YOU ADJUST THE 
	  testBWPDController.cxx FILE ACCORDINGLY
	  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	*/
	        rotationalPDController(
//			       adaptiveRotationalController(
	//       rotationalGyroObsPDController(
//		rotationalGyroObsPDControllerSwitch(
	                              m_measuredState, 
				      m_desiredState,
                                      m_controllerState, 
				      m_estimatedState,
				      timestep,
                                      rotationalTorque.ptr());
    }
    
    //rotationalTorque = rotationalTorque - (orientation.Inverse() * m_buoyantTorqueCorrection).crossProduct(math::Vector3::UNIT_Z);

    // Actually set motor values
    m_vehicle->applyForcesAndTorques(translationalForce, rotationalTorque);

    // Fire off events if needed
    // We use to be at depth now we aren't
    if (m_atDepth && !atDepth())
    {
        m_atDepth = false;
    }
    // We weren't at depth, now we are
    else if (!m_atDepth && atDepth())
    {
        publishAtDepth();
    }

    if (m_atOrientation && !atOrientation())
    {
        m_atOrientation = false;
    }
    // We weren't at depth, now we are
    else if (!m_atOrientation && atOrientation())
    {
        publishAtOrientation();
    }
        
    
    // Log values
    // old logging (for depth, PD rotational control, adaptive rotational
    // control)
    LOGGER.infoStream() << m_measuredState->quaternion[0] << " "
         << m_measuredState->quaternion[1] << " "
         << m_measuredState->quaternion[2] << " "
         << m_measuredState->quaternion[3] << " "
	 << m_measuredState->angularRate[0] << " "
	 << m_measuredState->angularRate[1] << " "
	 << m_measuredState->angularRate[2] << " "
         << m_measuredState->depth << " "
         << m_desiredState->quaternion[0] << " "
         << m_desiredState->quaternion[1] << " "
         << m_desiredState->quaternion[2] << " "
         << m_desiredState->quaternion[3] << " "
         << m_desiredState->angularRate[0] << " " 
	 << m_desiredState->angularRate[1] << " "
 	 << m_desiredState->angularRate[2] << " "
         << m_desiredState->depth << " "
         << m_desiredState->speed << " "
         << rotationalTorque[0] << " "
         << rotationalTorque[1] << " "
         << rotationalTorque[2] << " "
         << translationalForce[0] << " "
         << translationalForce[1] << " "
         << translationalForce[2] << " "
         << m_controllerState->adaptCtrlParams[0][0] << " "
         << m_controllerState->adaptCtrlParams[1][0] << " "
	 << m_controllerState->adaptCtrlParams[2][0] << " "
	 << m_controllerState->adaptCtrlParams[3][0] << " "
	 << m_controllerState->adaptCtrlParams[4][0] << " "
	 << m_controllerState->adaptCtrlParams[5][0] << " "
	 << m_controllerState->adaptCtrlParams[6][0] << " "
	 << m_controllerState->adaptCtrlParams[7][0] << " "
	 << m_controllerState->adaptCtrlParams[8][0] << " "
	 << m_controllerState->adaptCtrlParams[9][0] << " "
	 << m_controllerState->adaptCtrlParams[10][0] << " "
         << m_controllerState->adaptCtrlParams[11][0] << " "
                        << m_measuredState->velocity.x << " "
                        << m_measuredState->velocity.y << " "
                        << m_desiredState->velocity.x << " "
                        << m_desiredState->velocity.y << " "
                        << m_controllerState->useVelocityControl;
    
    // logging for gyro bias observer controller
    /*LOGGER.infoStream() << m_measuredState->quaternion[0] << " "
         << m_measuredState->quaternion[1] << " "
         << m_measuredState->quaternion[2] << " "
         << m_measuredState->quaternion[3] << " "
	 << m_measuredState->angularRate[0] << " "
	 << m_measuredState->angularRate[1] << " "
	 << m_measuredState->angularRate[2] << " "
         << m_measuredState->depth << " "
         << m_desiredState->quaternion[0] << " "
         << m_desiredState->quaternion[1] << " "
         << m_desiredState->quaternion[2] << " "
         << m_desiredState->quaternion[3] << " "
         << m_desiredState->angularRate[0] << " " 
	 << m_desiredState->angularRate[1] << " "
 	 << m_desiredState->angularRate[2] << " "
         << m_desiredState->depth << " "
         << m_desiredState->speed << " "
         << rotationalTorque[0] << " "
         << rotationalTorque[1] << " "
         << rotationalTorque[2] << " "
         << translationalForce[0] << " "
         << translationalForce[1] << " "
         << translationalForce[2] << " "
         << m_estimatedState->qhat[0] << " "
         << m_estimatedState->qhat[1] << " "
         << m_estimatedState->qhat[2] << " "
         << m_estimatedState->qhat[3] << " "
         << m_estimatedState->what[0] << " "
         << m_estimatedState->what[1] << " "
         << m_estimatedState->what[2] << " "
         << m_estimatedState->bhat[0] << " "
         << m_estimatedState->bhat[1] << " "
         << m_estimatedState->bhat[2];*/
}

void BWPDController::init(core::ConfigNode config)
{
    // Allocate state structures
    m_desiredState = new DesiredState();
    m_measuredState = new MeasuredState();
    m_controllerState = new ControllerState();
    m_estimatedState = new EstimatedState();
                           

    // Zero all the structs
    memset(m_desiredState, 0, sizeof(DesiredState));
    memset(m_measuredState, 0, sizeof(MeasuredState));
    memset(m_controllerState, 0, sizeof(ControllerState));
    memset(m_estimatedState, 0, sizeof(EstimatedState));

    m_depthThreshold = config["depthThreshold"].asDouble(DEPTH_TOLERANCE);
    m_orientationThreshold =
        config["orientationThreshold"].asDouble(ORIENTATION_THRESHOLD);
    
    // Set desired state
    m_desiredState->quaternion[0] = config["desiredQuaternion"][0].asDouble();
    m_desiredState->quaternion[1] = config["desiredQuaternion"][1].asDouble();
    m_desiredState->quaternion[2] = config["desiredQuaternion"][2].asDouble();
    m_desiredState->quaternion[3] = config["desiredQuaternion"][3].asDouble();

    m_desiredState->speed = config["desiredSpeed"].asDouble(0);
    m_desiredState->depth = config["desiredDepth"].asDouble(0);
    
    // Set controller state from config file (defaults hard coded)
	
	/* Rotational Gains */
	// nonlinear PD controller
    m_controllerState->angularPGain = config["angularPGain"].asDouble(1);
    m_controllerState->angularDGain = config["angularDGain"].asDouble(1);
	
	// nonlinear adaptive controller
	m_controllerState->adaptCtrlRotK = 
	    config["adaptCtrlRotK"].asDouble(1);
	m_controllerState->adaptCtrlRotLambda = 
	    config["adaptCtrlRotLambda"].asDouble(1);
	m_controllerState->adaptCtrlRotGamma = 
	    config["adaptCtrlRotGamma"].asDouble(1);
	m_controllerState->adaptCtrlParams.resize(12,1);
	m_controllerState->adaptCtrlParams[0][0] = config["adaptCtrlParams"][0].asDouble(0.5);
	m_controllerState->adaptCtrlParams[1][0] = config["adaptCtrlParams"][1].asDouble(0);
	m_controllerState->adaptCtrlParams[2][0] = config["adaptCtrlParams"][2].asDouble(-0.1);
	m_controllerState->adaptCtrlParams[3][0] = config["adaptCtrlParams"][3].asDouble(1);
	m_controllerState->adaptCtrlParams[4][0] = config["adaptCtrlParams"][4].asDouble(0);
	m_controllerState->adaptCtrlParams[5][0] = config["adaptCtrlParams"][5].asDouble(1);
	m_controllerState->adaptCtrlParams[6][0] = config["adaptCtrlParams"][6].asDouble(0);
	m_controllerState->adaptCtrlParams[7][0] = config["adaptCtrlParams"][7].asDouble(0);	
	m_controllerState->adaptCtrlParams[8][0] = config["adaptCtrlParams"][8].asDouble(0);
	m_controllerState->adaptCtrlParams[9][0] = config["adaptCtrlParams"][9].asDouble(1);
	m_controllerState->adaptCtrlParams[10][0] = config["adaptCtrlParams"][10].asDouble(2);
	m_controllerState->adaptCtrlParams[11][0] = config["adaptCtrlParams"][11].asDouble(2);

	// gyro bias observer controller
	// stuff from estimated state struct
	//qhat = [0,0,0,1]'
	m_estimatedState->qhat[0] = config["gyroQhat"][0].asDouble(0);
	m_estimatedState->qhat[1] = config["gyroQhat"][1].asDouble(0);
	m_estimatedState->qhat[2] = config["gyroQhat"][2].asDouble(0);
	m_estimatedState->qhat[3] = config["gyroQhat"][3].asDouble(1);
	//what = [0,0,0]'
	m_estimatedState->what[0] = config["gyroWhat"][0].asDouble(0);
	m_estimatedState->what[1] = config["gyroWhat"][1].asDouble(0);
	m_estimatedState->what[2] = config["gyroWhat"][2].asDouble(0);
	//bhat = [0,0,0]'
	m_estimatedState->bhat[0] = config["gyroBhat"][0].asDouble(0);
	m_estimatedState->bhat[1] = config["gyroBhat"][1].asDouble(0);
	m_estimatedState->bhat[2] = config["gyroBhat"][2].asDouble(0);
	//dqhat = [0,0,0,0]'
	m_estimatedState->dqhat[0] = config["gyroDqhat"][0].asDouble(0);
	m_estimatedState->dqhat[1] = config["gyroDqhat"][1].asDouble(0);
	m_estimatedState->dqhat[2] = config["gyroDqhat"][2].asDouble(0);
	m_estimatedState->dqhat[3] = config["gyroDqhat"][3].asDouble(0);
	//dbhat = [0,0,0]'
	m_estimatedState->dbhat[0] = config["gyroDbhat"][0].asDouble(0);
	m_estimatedState->dbhat[1] = config["gyroDbhat"][1].asDouble(0);
	m_estimatedState->dbhat[2] = config["gyroDbhat"][2].asDouble(0);
	// stuff from controller state struct
	m_controllerState->gyroObsGain = config["gyroObsGain"].asDouble(1);
	m_controllerState->gyroPDType = config["gyroPDType"].asInt(0);
	//qhatold = [0,0,0,1]'
	m_controllerState->qhatold[0] = config["gyroQhatold"][0].asDouble(0);
	m_controllerState->qhatold[1] = config["gyroQhatold"][1].asDouble(0);
	m_controllerState->qhatold[2] = config["gyroQhatold"][2].asDouble(0);
	m_controllerState->qhatold[3] = config["gyroQhatold"][3].asDouble(1);
	//whatold = [0,0,0]'
	m_controllerState->whatold[0] = config["gyroWhatold"][0].asDouble(0);
	m_controllerState->whatold[1] = config["gyroWhatold"][1].asDouble(0);
	m_controllerState->whatold[2] = config["gyroWhatold"][2].asDouble(0);
	//bhatold = [0,0,0]'
	m_controllerState->bhatold[0] = config["gyroBhatold"][0].asDouble(0);
	m_controllerState->bhatold[1] = config["gyroBhatold"][1].asDouble(0);
	m_controllerState->bhatold[2] = config["gyroBhatold"][2].asDouble(0);
	//dqhatold = [0,0,0,0]'
	m_controllerState->dqhatold[0] = config["gyroDqhatold"][0].asDouble(0);
	m_controllerState->dqhatold[1] = config["gyroDqhatold"][1].asDouble(0);
	m_controllerState->dqhatold[2] = config["gyroDqhatold"][2].asDouble(0);
	m_controllerState->dqhatold[3] = config["gyroDqhatold"][3].asDouble(0);
	//dbhatold = [0,0,0]'
	m_controllerState->dbhatold[0] = config["gyroDbhatold"][0].asDouble(0);
	m_controllerState->dbhatold[1] = config["gyroDbhatold"][1].asDouble(0);
	m_controllerState->dbhatold[2] = config["gyroDbhatold"][2].asDouble(0);

	/* Translationcal Gains */

    m_controllerState->depthControlType = config["depthControlType"].asInt(1);
    m_controllerState->dtMin = config["dtMin"].asDouble(0.001);
    m_controllerState->dtMax = config["dtMax"].asDouble(2.0);

    m_controllerState->speedPGain = config["speedPGain"].asInt(1);
    m_controllerState->sidewaysSpeedPGain =
        config["sidewaysSpeedPGain"].asInt(1);
    m_controllerState->velocityPGain = config["velocityPGain"].asDouble(1);
    m_controllerState->useVelocityControl = false;
    
    switch(m_controllerState->depthControlType)
    {
    case 1 :
        m_controllerState->depthPGain = config["depthPGain"].asDouble(1);
	    break;
	case 2 :
        m_controllerState->depthK = math::Vector2(config["depthKx"].asDouble(1.1),
    						config["depthKy"].asDouble(1.1));
        m_controllerState->depthL = math::Vector2(config["depthLx"].asDouble(1.1),
    						config["depthLy"].asDouble(1.1));


        m_controllerState->depthA = math::Matrix2(config["depthA1"].asDouble(0),
                                                config["depthA2"].asDouble(1),
                                                config["depthA3"].asDouble(0),
                                                config["depthA4"].asDouble(-.575));
        m_controllerState->depthB = math::Vector2(config["depthBx"].asDouble(0),
    						config["depthBy"].asDouble(0.05));
        m_controllerState->depthC = math::Vector2(config["depthCx"].asDouble(1),
    						config["depthCy"].asDouble(0));
	    break;
	case 3 :
        m_controllerState->depthA4 = math::Matrix4(config["depthOCIA4"][0].asDouble(0),
                                                   config["depthOCIA4"][1].asDouble(0),
                                                   config["depthOCIA4"][2].asDouble(0),
                                                   config["depthOCIA4"][3].asDouble(0),
                                                   config["depthOCIA4"][4].asDouble(0),
                                                   config["depthOCIA4"][5].asDouble(0),
                                                   config["depthOCIA4"][6].asDouble(0),
                                                   config["depthOCIA4"][7].asDouble(0),
                                                   config["depthOCIA4"][8].asDouble(0),
                                                   config["depthOCIA4"][9].asDouble(0),
                                                   config["depthOCIA4"][10].asDouble(0),
                                                   config["depthOCIA4"][11].asDouble(0),
                                                   config["depthOCIA4"][12].asDouble(0),
                                                   config["depthOCIA4"][13].asDouble(0),
                                                   config["depthOCIA4"][14].asDouble(0),
                                                   config["depthOCIA4"][15].asDouble(0));

        m_controllerState->depthB4 = math::Vector4(config["depthOCIB4"][0].asDouble(0),
                                                   config["depthOCIB4"][1].asDouble(0),
                                                   config["depthOCIB4"][2].asDouble(0),
                                                   config["depthOCIB4"][3].asDouble(0));

        m_controllerState->depthC4 = math::Vector4(config["depthOCIC4"][0].asDouble(0),
                                                   config["depthOCIC4"][1].asDouble(0),
                                                   config["depthOCIC4"][2].asDouble(0),
                                                   config["depthOCIC4"][3].asDouble(0));
        m_estimatedState->xHat4Depth = math::Vector4(0,0,0,0);
            break;
        case 4 :
	case 6 :
        m_controllerState->depthA4 = math::Matrix4(config["depthOCIA4"][0].asDouble(0),
                                                   config["depthOCIA4"][1].asDouble(0),
                                                   config["depthOCIA4"][2].asDouble(0),
                                                   config["depthOCIA4"][3].asDouble(0),
                                                   config["depthOCIA4"][4].asDouble(0),
                                                   config["depthOCIA4"][5].asDouble(0),
                                                   config["depthOCIA4"][6].asDouble(0),
                                                   config["depthOCIA4"][7].asDouble(0),
                                                   config["depthOCIA4"][8].asDouble(0),
                                                   config["depthOCIA4"][9].asDouble(0),
                                                   config["depthOCIA4"][10].asDouble(0),
                                                   config["depthOCIA4"][11].asDouble(0),
                                                   config["depthOCIA4"][12].asDouble(0),
                                                   config["depthOCIA4"][13].asDouble(0),
                                                   config["depthOCIA4"][14].asDouble(0),
                                                   config["depthOCIA4"][15].asDouble(0));

        m_controllerState->depthB4 = math::Vector4(config["depthOCIB4"][0].asDouble(0),
                                                   config["depthOCIB4"][1].asDouble(0),
                                                   config["depthOCIB4"][2].asDouble(0),
                                                   config["depthOCIB4"][3].asDouble(0));

        m_controllerState->depthC4 = math::Vector4(config["depthOCIC4"][0].asDouble(0),
                                                   config["depthOCIC4"][1].asDouble(0),
                                                   config["depthOCIC4"][2].asDouble(0),
                                                   config["depthOCIC4"][3].asDouble(0));
        m_estimatedState->xHat4Depth = math::Vector4(0,0,0,0);
        break;
    case 5 :
        m_controllerState->depthKi = config["depthKi"].asDouble(1);
        m_controllerState->depthKp = config["depthKp"].asDouble(10);
        m_controllerState->depthKd = config["depthKd"].asDouble(4);
        m_controllerState->depthSumError = config["depthSumError"].asDouble(1);
        m_controllerState->depthPrevX = config["depthPrevX"].asDouble(1);
        break;
	default :
	    //depthControlSignal=depthPController(measuredState,desiredState,controllerState);
	    break;
    }

    
    m_controllerState->inertiaEstimate[0][0] =
        config["inertia"][0][0].asDouble(0.201);
    m_controllerState->inertiaEstimate[0][1] =
        config["inertia"][0][1].asDouble(0);
    m_controllerState->inertiaEstimate[0][2] =
        config["inertia"][0][2].asDouble(0);
    
    m_controllerState->inertiaEstimate[1][0] =
        config["inertia"][1][0].asDouble(0);
    m_controllerState->inertiaEstimate[1][1] =
        config["inertia"][1][1].asDouble(1.288);
    m_controllerState->inertiaEstimate[1][2] =
        config["inertia"][1][2].asDouble(0);
    
    m_controllerState->inertiaEstimate[2][0] =
        config["inertia"][2][0].asDouble(0);
    m_controllerState->inertiaEstimate[2][1] =
        config["inertia"][2][1].asDouble(0);
    m_controllerState->inertiaEstimate[2][2] =
        config["inertia"][2][2].asDouble(1.288);

        
    //Set estimated struct
    //we don't know what the state is, so guess (0,0).  
    m_estimatedState->xHat2Depth.x = 0;
    m_estimatedState->xHat2Depth.y = 0;
    
    // old logging header
    LOGGER.infoStream() << "% M-Quat M-AngRate M-Depth D-Quat D-AngRate"
        << " D-Depth D-Speed RotTorq"
        << " TranForce AdaptCtrlParams(12 values) M-Vel D-Vel Vel-InUse Time"
    // for adaptive rotational controller gains`
     	<< " (AdaptGains- K:" << m_controllerState->adaptCtrlRotK 
        << " Gamma: " << m_controllerState->adaptCtrlRotGamma
	<< " Lambda: " << m_controllerState->adaptCtrlRotLambda << ")";
    //gyro bias observer controller header
//    LOGGER.info("% M-Quat M-AngRate M-Depth D-Quat D-AngRate D-Depth D-Speed RotTorq TransForce qhat what bhat");

    if (config["holdCurrentHeading"].asInt(0))
        m_holdStartOrientation = true;
    if (config["holdCurrentDepth"].asInt(0))
        holdCurrentDepth();
    m_initializationPause = config["initializationPause"].asDouble(0);

}

void BWPDController::publishAtDepth()
{
    m_atDepth = true;
    math::NumericEventPtr event(new math::NumericEvent());
    event->number = m_measuredState->depth;
    publish(IController::AT_DEPTH, event);
}

void BWPDController::publishAtOrientation()
{
    m_atOrientation = true;
    math::OrientationEventPtr event(new math::OrientationEvent());
    event->orientation = math::Quaternion(m_measuredState->quaternion);
    publish(IController::AT_ORIENTATION, event);        
}


/** Sets desired velocity and velocity based control for new controllers */
void BWPDController::setDesiredVelocity(math::Vector2 velocity, int frame)
{
    std::cerr << "BWPDController: NOT YET IMPLEMENTED" << std::endl;
}
    
/** Sets desired position and position based control for new controllers */
void BWPDController::setDesiredPosition(math::Vector2 position, int frame)
{
     std::cerr << "BWPDController::setDesiredPosition(): NOT IMPLEMENTED"
               << std::endl;
}
 
/** Sets a desired position and velocity for controling of both simultaneously */
void BWPDController::setDesiredPositionAndVelocity(math::Vector2 position,
                                                   math::Vector2 velocity)
{
     std::cerr << "BWPDController::setDesiredPositionAndVelocity(): NOT IMPLEMENTED"
               << std::endl;
}

/** Gets desired velocity */
math::Vector2 BWPDController::getDesiredVelocity(int frame)
{
    std::cerr << "BWPDController::getDesiredVelocity(): NOT IMPLEMENTED" << std::endl;
    return math::Vector2::ZERO;
}

/** Gets desired position */
math::Vector2 BWPDController::getDesiredPosition(int frame)
{
    std::cerr << "BWPDController::getDesiredPosition(): NOT IMPLEMENTED" << std::endl;
    return math::Vector2::ZERO;
}

bool BWPDController::atPosition()
{
    std::cerr << "BWPDController::atPositioN(): NOT IMPLEMENTED" << std::endl;
    return 0;
}

bool BWPDController::atVelocity()
{
    std::cerr << "BWPDController::atVelocity(): NOT IMPLEMENTED" << std::endl;
    return 0;
}

} // namespace bwpd        
} // namespace control
} // namespace ram
