

#ifndef RAM_DVL_ACCELEROMETER_ESTIMATIONMODULE_H
#define RAM_DVL_ACCELEROMETER_ESTIMATIONMODULE_H


// STD Includes

// Library Includes
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

// Project Includes
#include "estimation/include/EstimatedState.h"
#include "estimation/include/EstimationModule.h"

#include "core/include/ConfigNode.h"
#include "core/include/Event.h"
#include "core/include/EventPublisher.h"
#include "core/include/EventConnection.h"

#include "vehicle/include/Events.h"
#include "vehicle/include/device/IVelocitySensor.h"
#include "vehicle/include/device/IMU.h"

#include "math/include/Math.h"
#include "math/include/MatrixN.h"
#include "math/include/VectorN.h"
#include "math/include/Vector3.h"
#include "math/include/Vector2.h"
#include "math/include/Quaternion.h"

namespace ram{
namespace estimation{

class DVLAccelerometerEstimator : public EstimationModule
{
 public:
  
  DVLAccelerometerEstimator(core::EventHubPtr eventHub =  core::EventHubPtr(), 
                            EstimatedStatePtr estState  = EstimatedStatePtr());

  void update_velocity(core::EventPtr event);
  void update_acceleration(core::EventPtr event);
  
  math::MatrixN get_Estimate();
  math::MatrixN get_Covariance();
  math::MatrixN get_Identity();

  virtual ~DVLAccelerometerEstimator();
  virtual void update(core::EventPtr event);
    
 protected:

  std::string m_name;
  EstimatedStatePtr m_estimatedState;
  core::EventConnectionPtr v_connection;
  core::EventConnectionPtr a_connection;
  
  math::MatrixN estimate;                                                 
  math::MatrixN covariance;                       
  math::MatrixN identity;
  
};

}

}


#endif
