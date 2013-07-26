/*
 *Kalman Filter for DVL and Accelerometer
 */

#include "estimation/include/DVLAccelerometerEstimator.h"

namespace ram {

namespace estimation {
  
  DVLAccelerometerEstimator::DVLAccelerometerEstimator(core::EventHubPtr eventHub,
                       EstimatedStatePtr estState):
    EstimationModule(eventHub, "DVLAccelerometerEstimator",estState,
                     vehicle::device::IVelocitySensor::RAW_UPDATE),
      v_connection(eventHub->subscribeToType(
                       vehicle::device::IVelocitySensor::RAW_UPDATE, 
                       boost::bind(&ram::estimation::DVLAccelerometerEstimator::update_velocity, this, _1))),
      a_connection(eventHub->subscribeToType(
                       vehicle::device::IIMU::RAW_UPDATE,
                       boost::bind(&ram::estimation::DVLAccelerometerEstimator::update_acceleration, this, _1))),
      estimate(0.0, 6, 1),
      covariance(0.0, 6, 6),
      identity(0.0, 6, 6)
  {
    int i;
    for (i = 0; i < 6; i++){
      identity[i][i] = 1; 
      covariance[i][i] = 1;
    }
  }

  DVLAccelerometerEstimator::~DVLAccelerometerEstimator()
  {
  }
 
  math::MatrixN DVLAccelerometerEstimator::get_Estimate(){
    return DVLAccelerometerEstimator::estimate;
  }
  
  math::MatrixN DVLAccelerometerEstimator::get_Covariance(){
    return DVLAccelerometerEstimator::covariance;
  }

  math::MatrixN DVLAccelerometerEstimator::get_Identity(){
    return DVLAccelerometerEstimator::identity;
  }
  
  void DVLAccelerometerEstimator::update_velocity(core::EventPtr event){

   vehicle::RawDVLDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawDVLDataEvent>(event);

    // return if the cast failed and let people know about it.
   if(!ievent){
        return;
    }

       // we want to work with a vector3 so we can rotate in 3 dimensions
    math::Vector3 vel3_b(ievent->velocity_b[0], ievent->velocity_b[1], 0);

    math::Quaternion orientation = m_estimatedState->getEstimatedOrientation();

    // rotate the body frame velocity to the inertial frame
    // this is valid for any orientation, dont need to be level
    math::Vector3 vel3_n = orientation.UnitInverse() * vel3_b;

    double x_in = vel3_n[0];
    double y_in = vel3_n[1];
    double dt = ievent->timestep;

      math::MatrixN state_transition(0.0, 6, 6);// state transition model
      state_transition[0][0] = 1;
      state_transition[0][1] = dt;
      state_transition[0][2] = .5*dt*dt;
      state_transition[1][1] = 1;
      state_transition[1][2] = dt;
      state_transition[2][2] = 1;      
      
      state_transition[3][3] = 1;
      state_transition[3][4] = dt;
      state_transition[3][5] = .5*dt*dt;
      state_transition[4][4] = 1;
      state_transition[4][5] = dt;
      state_transition[5][5] = 1;

      math::MatrixN observation_matrix(0.0, 2, 6);// observation model      
      observation_matrix[0][1] = 1;
      observation_matrix[1][4] = 1;
      
      math::MatrixN Q(0.0, 6,6);// process covariance
      Q[0][0] = .001;
      Q[1][1] = .00009;
      Q[2][2] = .000009;
      Q[3][3] = .001;
      Q[4][4] = .00009;
      Q[5][5] = .000009;
            
      math::MatrixN R(0.001, 2, 2);// measurement covariance
      R[0][0] = .006915;
      R[0][1] = .006915;
      R[1][0] = .006915;
      R[1][1] = .007258;

      
      /*
       *Measurement Prediction
       */
      estimate = state_transition * estimate;
      covariance = state_transition * covariance * state_transition.transpose() + Q;
      
      /*
       *Measurement Update
       */
      math::MatrixN zk(0.0, 2, 1);
      zk[0][0] = x_in;
      zk[1][0] = y_in;
      
      //Measurement Residual
      math::MatrixN y(zk - (observation_matrix * estimate));
      
      //Covariance Residual
      math::MatrixN S(observation_matrix * covariance * observation_matrix.transpose() + R);
      
      //Optimal Kalman gain
      math::MatrixN K(covariance * observation_matrix.transpose() * S.invert());
      
      estimate = estimate + K * y;
      covariance = (identity - (K * observation_matrix)) * covariance;
      
      math::Vector2 position(estimate[0][0], estimate[3][0]);
      math::Vector2 velocity(estimate[1][0], estimate[4][0]);
      math::Vector3 acceleration(estimate[2][0], estimate[5][0], m_estimatedState->getEstimatedLinearAccel()[2]);

      m_estimatedState->setEstimatedPosition(position);
      m_estimatedState->setEstimatedVelocity(velocity);
      m_estimatedState->setEstimatedLinearAccel(acceleration);

    }

    void DVLAccelerometerEstimator::update_acceleration(core::EventPtr event){
      
        vehicle::RawIMUDataEventPtr ievent =
            boost::dynamic_pointer_cast<vehicle::RawIMUDataEvent>(event);

        /* Return if the cast failed and let people know about it. */
        if(!ievent){
            return;
        }
        
        RawIMUData newState = ievent->rawIMUData;
        

        double x_in = newState.accelX;
        double y_in = newState.accelY;
        double dt = ievent->timestep;

      math::MatrixN state_transition(0.0, 6, 6);// state transition model
      state_transition[0][0] = 1;
      state_transition[0][1] = dt;
      state_transition[0][2] = .5*dt*dt;
      state_transition[1][1] = 1;
      state_transition[1][2] = dt;
      state_transition[2][2] = 1;

      state_transition[3][3] = 1;
      state_transition[3][4] = dt;
      state_transition[3][5] = .5*dt*dt;
      state_transition[4][4] = 1;
      state_transition[4][5] = dt;
      state_transition[5][5] = 1;


      math::MatrixN observation_matrix(0.0, 2, 6);// observation model              
      observation_matrix[0][2] = 1;
      observation_matrix[1][5] = 1;
      
      math::MatrixN Q(0.0, 6, 6);// process covariance
      Q[0][0] = .001;
      Q[1][1] = .00009;
      Q[2][2] = .000009;
      Q[3][3] = .1;
      Q[4][4] = .00009;
      Q[5][5] = .000009;

      math::MatrixN R(0.001, 2, 2);// measurement covariance
      R[0][0] = 1.58;
      R[0][1] = -0.68;
      R[1][0] = -0.68;
      R[1][1] = 4.66;

      /*                                                                                                                                                     
       *Measurement Prediction                                                                                                                               
       */
      estimate = state_transition * estimate;
      covariance = state_transition * covariance * state_transition.transpose() + Q;
      
      /*
       *Measurement Update
       */
      math::MatrixN zk(0.0, 2, 1);
      zk[0][0] = x_in;
      zk[1][0] = y_in;

      //Measurement Residual
      math::MatrixN y(zk - (observation_matrix * estimate));

      //Covariance Residual
      math::MatrixN S(observation_matrix * covariance * observation_matrix.transpose() + R);

      //Optimal Kalman gain
      math::MatrixN K(covariance * observation_matrix.transpose() * S.invert());

      estimate = estimate + K * y;
      covariance = (identity - (K * observation_matrix)) * covariance;

      math::Vector2 position(estimate[0][0], estimate[3][0]);
      math::Vector2 velocity(estimate[1][0], estimate[4][0]);
      math::Vector3 acceleration(estimate[2][0], estimate[5][0], m_estimatedState->getEstimatedLinearAccel()[2]);

      m_estimatedState->setEstimatedPosition(position);
      m_estimatedState->setEstimatedVelocity(velocity);
      m_estimatedState->setEstimatedLinearAccel(acceleration);
    } 
    
    void DVLAccelerometerEstimator::update(core::EventPtr event)
    {
    }
  }
}
