/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/src/NonlinearPDRotationalController.cpp
 */

// Project Includes
#include "estimation/include/IStateEstimator.h"
#include "control/include/AdaptiveRotationalController.h"
#include "control/include/ControllerMaker.h"
#include "math/include/Matrix2.h"
#include "math/include/Matrix3.h"
#include "math/include/MatrixN.h"

namespace ram {
namespace control {

static RotationalControllerImpMakerTemplate<AdaptiveRotationalController>
registerAdaptiveRotationalController("AdaptiveRotationalController");

AdaptiveRotationalController::AdaptiveRotationalController(
    core::ConfigNode config) :
    RotationalControllerBase(config),
    m_dtMin(config["dtMin"].asDouble(0.0)),
    m_dtMax(config["dtMax"].asDouble(1.0)),
    m_rotLambda(config["rotLambda"].asDouble(1.0)),
    m_rotGamma(config["rotGamma"].asDouble(1.0)),
    m_rotK(config["rotK"].asDouble(1.0)),
    m_params(math::MatrixN(12,1))
{
    m_params[0][0] = config["adaptParams"][0].asDouble(0.5);
    m_params[1][0] = config["adaptParams"][1].asDouble(0);
    m_params[2][0] = config["adaptParams"][2].asDouble(-0.1);
    m_params[3][0] = config["adaptParams"][3].asDouble(1);
    m_params[4][0] = config["adaptParams"][4].asDouble(0);
    m_params[5][0] = config["adaptParams"][5].asDouble(1);
    m_params[6][0] = config["adaptParams"][6].asDouble(0);
    m_params[7][0] = config["adaptParams"][7].asDouble(0);
    m_params[8][0] = config["adaptParams"][8].asDouble(0);
    m_params[9][0] = config["adaptParams"][9].asDouble(1);
    m_params[10][0] = config["adaptParams"][10].asDouble(2);
    m_params[11][0] = config["adaptParams"][11].asDouble(2);

}

math::Vector3 AdaptiveRotationalController::rotationalUpdate(
    double timestep,
    estimation::IStateEstimatorPtr estimator,
    control::DesiredStatePtr desiredState)
{
    /* Prevent mult, div by zero bugs */
    clip(timestep, m_dtMin, m_dtMax);

    /* Using shortened variable names due to complicated computation.
     * d as suffix means desired
     * d as prefix means derivative
     * m as suffix means measured
     * q means quaternion
     * w means angular rate (omega)
     * tilde denotes an error quantity
     */
    math::Quaternion qd(desiredState->getDesiredOrientation());
    math::Quaternion q(estimator->getEstimatedOrientation());
    math::Vector3 wd(desiredState->getDesiredAngularRate());
    math::Vector3 w(estimator->getEstimatedAngularRate());

/****************************
       propagate desired states 
*****************************/

// integrate desired angular velocity
// derivative of angular rate desired
    math::Vector3 dwd(0,0,0);
// simple integration
    wd = wd+dwd*timestep;

// integrate desired angular position
// compute derivative of quaternion desired
    math::Quaternion dqd = qd.derivative(wd);
// simple integration
    qd = qd+dqd*timestep;
// fix numerical drift
    qd.normalise();

    // update the desired state
    desiredState->setDesiredAngularRate(wd);
    desiredState->setDesiredOrientation(qd);

/****************************
       compute error metrics
*****************************/

// compute error quaternion
    math::Quaternion qc_tilde = q.errorQuaternion(qd);

// compute rotation matrix (attitude deviation matrix?)
    math::Matrix3 RotMatc_tilde;
    qc_tilde.ToRotationMatrix(RotMatc_tilde);

// extract vector portion of qc_tilde
    math::Vector3 epsilon_c_tilde(qc_tilde.x, qc_tilde.y, qc_tilde.z);

// compute composite error metrics
    math::Vector3 wr = RotMatc_tilde*wd-(m_rotLambda)*epsilon_c_tilde;
    math::Vector3 shat = w-wr;

// compute angular rate error
    math::Vector3 wc_tilde = w-RotMatc_tilde*wd;

// compute derivative of wr
    math::Matrix3 S;//temp skew symmetric matrix
    S.ToSkewSymmetric(epsilon_c_tilde);

    math::Matrix3 Q1;//temp Q1 matrix (subset of Q matrix)
    Q1[0][0]=qc_tilde.w+S[0][0];
    Q1[0][1]=S[0][1];
    Q1[0][2]=S[0][2];
    Q1[1][0]=S[1][0];
    Q1[1][1]=qc_tilde.w+S[1][1];
    Q1[1][2]=S[1][2];
    Q1[2][0]=S[2][0];
    Q1[2][1]=S[2][1];
    Q1[2][2]=qc_tilde.w+S[2][2];

    S.ToSkewSymmetric(wc_tilde);
    math::Vector3 dwr = RotMatc_tilde*dwd - (m_rotLambda)*Q1*wc_tilde;


/**********************************
  compute parameterization matrix
**********************************/

// rotation matrix from vehicle quaternion
    math::Matrix3 Rot;
    q.ToRotationMatrix(Rot);

// the dreaded parameterization matrix
    math::MatrixN Y(3,12);

// inertia terms
    Y[0][0] = dwr[0];
    Y[0][1] = dwr[1] - w[0]*wr[2];
    Y[0][2] = dwr[2] + w[0]*wr[1];
    Y[1][0] = w[0]*wr[2];
    Y[1][1] = dwr[0] + w[1]*wr[2];
    Y[1][2] = -w[0]*wr[0] + w[2]*wr[2];
    Y[2][0] = -w[0]*wr[1];
    Y[2][1] = w[0]*wr[0] - w[1]*wr[1];
    Y[2][2] = dwr[0] - w[2]*wr[1];

// more inertia terms
    Y[0][3] = -w[1]*wr[2];
    Y[0][4] = w[1]*wr[1] - w[2]*wr[2];
    Y[0][5] = w[2]*wr[1];
    Y[1][3] = dwr[1];
    Y[1][4] = dwr[2] - w[1]*wr[0];
    Y[1][5] = -w[2]*wr[0];
    Y[2][3] = w[1]*wr[0];
    Y[2][4] = dwr[1] + w[2]*wr[0];
    Y[2][5] = dwr[2];

// buoyancy terms
    Y[0][6] = 0;
    Y[0][7] = -Rot[2][2];
    Y[0][8] = Rot[1][2];
    Y[1][6] = Rot[2][2];
    Y[1][7] = 0;
    Y[1][8] = -Rot[0][2];
    Y[2][6] = -Rot[1][2];
    Y[2][7] = Rot[0][2];
    Y[2][8] = 0;

// drag terms
    Y[0][9]  = -w[0]*fabs(w[0]);
    Y[0][10] = 0;
    Y[0][11] = 0;
    Y[1][9]  = 0;
    Y[1][10] = -w[1]*fabs(w[1]);
    Y[1][11] = 0;
    Y[2][9]  = 0;
    Y[2][10] = 0;
    Y[2][11] = -w[2]*fabs(w[2]);

/**********************************
  adaptation law
**********************************/

// use parameter adaptation law
    math::MatrixN dahat = -(m_rotGamma)*Y.transpose()*shat;

// integrate parameter estimates & store in controllerState
    m_params = m_params + dahat*timestep;

    /* Implement a dead zone to prevent parameter drift.
     * Limits are currently hardcoded.
     */

    clip(m_params[0][0], -2.0, 2.0);
    clip(m_params[1][0], -1.0, 1.0);
    clip(m_params[2][0], -0.5, 0.5);

    clip(m_params[3][0], -2.0, 2.0);
    clip(m_params[4][0], -1.0, 1.0);
    clip(m_params[5][0], -2.0, 2.0);

    clip(m_params[6][0], -1.0, 1.0);
    clip(m_params[7][0], -1.0, 1.0);
    clip(m_params[8][0], -1.0, 1.0);

    clip(m_params[9][0], 0.0, 5.0);
    clip(m_params[10][0], 0.0, 4.0);
    clip(m_params[11][0], 0.0, 5.0);

/**********************************
  control law
**********************************/

    math::MatrixN adaptiveTerm = Y*m_params;

    math::Vector3 output(adaptiveTerm[0][0],adaptiveTerm[1][0],adaptiveTerm[2][0]);
    output = output-(m_rotK)*shat;
 
    return math::Vector3(output[0], output[1], output[2]);
}

void AdaptiveRotationalController::clip(double &var, double min, double max)
{
    if (var > max)
        var = max;
    else if (var < min)
        var = min;
}

} // namespace control
} // namespace ram
