/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/src/GyroObserverPDControllerSwitch.cpp
 */

// Library Includes
#include <iostream>

// Project Includes
#include "control/include/GyroObserverPDControllerSwitch.h"
#include "control/include/ControllerMaker.h"
#include "math/include/Matrix2.h"
#include "math/include/Matrix3.h"
#include "math/include/MatrixN.h"
#include "math/include/Helpers.h"

namespace ram {
namespace control {

static RotationalControllerImpMakerTemplate<GyroObserverPDControllerSwitch>
registerGyroObserverPDControllerSwitch("GyroObserverPDControllerSwitch");

GyroObserverPDControllerSwitch::GyroObserverPDControllerSwitch(
    core::ConfigNode config) :
    RotationalControllerBase(config),
    m_dtMin(config["dtMin"].asDouble(0.0)),
    m_dtMax(config["dtMax"].asDouble(1.0)),
    m_gyroObsGain(config["gyroObsGain"].asDouble(0.0)),
    m_angularPGain(config["kp"].asDouble(0.0)),
    m_angularDGain(config["kd"].asDouble(0.0)),
    m_gyroPDType(config["gyroPDType"].asInt(0)),
    m_bHatOld(math::Vector3::ZERO),
    m_dbHatOld(math::Vector3::ZERO),
    m_wHatOld(math::Vector3::ZERO),
    m_qHatOld(math::Quaternion::IDENTITY),
    m_dqHatOld(math::Quaternion::ZERO),
    m_inertia(math::Matrix3(config["inertia"][0][0].asDouble(0.201),
                            config["inertia"][0][1].asDouble(0.0),
                            config["inertia"][0][2].asDouble(0.0),
                            config["inertia"][1][0].asDouble(0.0),
                            config["inertia"][1][1].asDouble(1.288),
                            config["inertia"][1][2].asDouble(0.0),
                            config["inertia"][2][0].asDouble(0.0),
                            config["inertia"][2][1].asDouble(0.0),
                            config["inertia"][2][2].asDouble(1.288)))
{


}

math::Vector3 GyroObserverPDControllerSwitch::rotationalUpdate(
    double timestep,
    estimation::IStateEstimatorPtr estimator,
    control::DesiredStatePtr desiredState)
{
    clip(timestep, m_dtMin, m_dtMax);

    // grab data and put in OGRE format
    math::Matrix3 J(m_inertia);
    math::Quaternion q_meas(estimator->getEstimatedOrientation());
    math::Quaternion q_des(desiredState->getDesiredOrientation());
    math::Vector3 w(estimator->getEstimatedAngularRate());

    // ensure quaternions are of unit length
    q_meas.normalise();
    q_des.normalise();

    /********************/
    /***** OBSERVER *****/
    /********************/

    // compute attitude prediction error
    math::Quaternion qto;//QTO = Quaternion Tilde Observer
    qto = q_meas.errorQuaternion(m_qHatOld);

    // break up quaternion into vector and scalar parts for later convenience
    math::Vector3 epsilon_to(qto.x, qto.y, qto.z);
    double eta_to = qto.w;

    // estimate current gyro bias and gyro bias rate
    // compute current gyro bias rate from attitude prediction error
    math::Vector3 dbHat = -0.5 * math::sign(eta_to) * epsilon_to;

    // integrate (trapezoidal used) to estimate gyro bias
    math::Vector3 bHat = m_bHatOld + 0.5 * (dbHat + m_dbHatOld) * timestep;
    
    // estimate current angular rate
    math::Vector3 wHat = w - bHat;

    // estimate current attitude
    // first create a blank rotation matrix
    math::Matrix3 Rtranspose;

    // compute R'(qto)  (note that OGRE's rotation matrix is the transpose of R(q) )
    qto.ToRotationMatrix(Rtranspose);

    //now create a blank Q matrix
    math::MatrixN Q(4,3);

    // compute Q(qhatold)
    m_qHatOld.toQ(&Q);

    // compute attitude estimate rate from observer dynamics eq
    math::MatrixN dqHatN(4,1);
    
    // make more things MatrixN
    math::MatrixN RtranN(Rtranspose);
    math::MatrixN wHatN(wHat);
    math::MatrixN epsilon_toN(epsilon_to);
    dqHatN = 0.5*Q*RtranN*(wHatN + m_gyroObsGain*math::sign(eta_to)*epsilon_toN);
    
    math::Quaternion dqHat(dqHatN[0][0], dqHatN[1][0], dqHatN[2][0], dqHatN[3][0]);

    // integrate (trapezoidal used) to estimate attitude 
    math::Quaternion qHat = m_qHatOld+0.5*(dqHat+m_dqHatOld)*timestep;
    qHat.normalise();

    // copy observer estimates into persistant storage
    m_qHatOld = qHat;
    m_wHatOld = wHat;
    m_bHatOld = bHat;
    m_dqHatOld = dqHat;
    m_dbHatOld = dbHat;

    /**********************/
    /***** CONTROLLER *****/
    /**********************/

    /* Notation
     * qtc = quaternion tilde controller
     * epsilon_tc - vector part of quaternion
     * eta_tc - scalar part of quaternion
     * w_error - angular rate error
     * w_tilde - gyroscopic term matrix
     */

    math::Quaternion qtc;
    math::Vector3 w_error;
    double kp = m_angularPGain;
    double kd = m_angularDGain;

    switch(m_gyroPDType){
    case 1:
        // calculate error quaternion from estimator values
        qtc = math::Quaternion(q_meas.errorQuaternion(q_des));
        // use observer estimate of angular rate
        w_error = math::Vector3(wHat);
        break;

    case 2:
        // calculate error quaternion from observer estimates
        qtc = math::Quaternion(qHat.errorQuaternion(q_des));
        // use observer estimate of angular rate
        w_error = math::Vector3(wHat);
        break;

    default:
    case 0:
        // calculate quaternion from estimator values
        qtc = math::Quaternion(q_meas.errorQuaternion(q_des));
        // use the angular rate from the estimator
        w_error = math::Vector3(estimator->getEstimatedAngularRate());
        break;
    }
    
    // break up quaternion into vector and scalar parts for later convenience
    math::Vector3 epsilon_tc(qtc.x, qtc.y, qtc.z);
    double eta_tc = qtc.w;

    math::Matrix3 w_tilde;
    w_tilde.ToSkewSymmetric(w_error);

    // compute the control signal
    return - kp*J*math::sign(eta_tc)*epsilon_tc - kd*J*w_error + w_tilde*J*w_error;
}

void GyroObserverPDControllerSwitch::clip(double &var, double min, double max)
{
    if (var > max)
        var = max;
    else if (var < min)
        var = min;
}

} // namespace control
} // namespace ram
