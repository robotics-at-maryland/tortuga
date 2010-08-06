/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Gland <jgland@ssl.umd.edu>
 * File:  packages/math/include/Helpers.h
 */

#ifndef RAM_MATH_H_07_07_2007
#define RAM_MATH_H_07_07_2007

// Must Be Included last
#include "math/include/Export.h"
#include "math/include/Matrix2.h"

namespace ram {
namespace math {

void RAM_EXPORT tilde(double w[3], double * ptilde);
double RAM_EXPORT sign(double variable);
void RAM_EXPORT createOmega(double w[3], double * pOMEGA);

void RAM_EXPORT normalize3x1(double* pvec);
void RAM_EXPORT normalize4x1(double* pvec);

double RAM_EXPORT magnitude3x1(double vec[3]);
double RAM_EXPORT magnitude4x1(double vec[4]);

double RAM_EXPORT det2x2(double a[2][2]);
double RAM_EXPORT det3x3(double a[3][3]);

void RAM_EXPORT invert3x3(double mat[3][3], double * pInverted);
void RAM_EXPORT matrixAdd3x1and3x1(double a[3], double b[3], double* const presult);
void RAM_EXPORT matrixAdd4x1and4x1(double a[4], double b[4], double* const presult);
void RAM_EXPORT matrixMult3x1byScalar(double vec[3], double scalar,
                           double* const presult);
void RAM_EXPORT matrixMult4x1byScalar(double vec[4], double scalar,
                           double* const presult);
void RAM_EXPORT matrixMult3x3byScalar(double matrix[3][3], double scalar,
                           double* const presult);
void RAM_EXPORT matrixMult3x1by3x3(double bigMatrix[3][3], double littleMatrix[3],
                        double* const presult);
void RAM_EXPORT matrixMult4x4by4x1(const double big[4][4],
                                   const double little[4],
                                   double* const presult);
void RAM_EXPORT matrixMult3x3by3x3(double a[3][3], double b[3][3], double* presult);
void RAM_EXPORT crossProduct3x1by3x1(double a[3], double b[3], double * presult);

void RAM_EXPORT eulerAxisToQuaternion(double e[3], double phi, double* const pq);
void RAM_EXPORT quaternionToEulerAxis(double q[4], double* pphi, double* pe);
void RAM_EXPORT findErrorQuaternion(double qc[4], double q[4], double * pqe);
void RAM_EXPORT rotationMatrixFromQuaternion(double q[4], double * pRot);
void RAM_EXPORT quaternionFromEulerAxis(double e[3], double et, double * pQ);
void RAM_EXPORT quaternionCrossProduct(const double q1[4],
                                       const double q2[4], double * pQ);

double RAM_EXPORT getDistance(double a[3], double b[3]);
void RAM_EXPORT getQuaternionErrorMatrix(double q[4], double * pMatrix);
void RAM_EXPORT quaternionFromnCb(double nCb[3][3], double * pQ);

void RAM_EXPORT rotationRoll(double phi, double * pMatrix);
void RAM_EXPORT rotationPitch(double theta, double * pMatrix);
void RAM_EXPORT rotationYaw(double psi, double * pMatrix);


Matrix2 RAM_EXPORT bRn(double radians);

Matrix2 RAM_EXPORT nRb(double radians);

} // namespace math
} // namespace ram
    
#endif // RAM_MATH_H_07_07_2007
