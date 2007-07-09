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

namespace ram {
namespace math {

void tilde(double w[3], double * ptilde);
double sign(double variable);
void createOmega(double w[3], double * pOMEGA);

void normalize3x1(double* pvec);
void normalize4x1(double* pvec);

double magnitude3x1(double vec[3]);
double magnitude4x1(double vec[4]);

double det2x2(double a[2][2]);
double det3x3(double a[3][3]);

void invert3x3(double mat[3][3], double * pInverted);
void matrixAdd3x1and3x1(double a[3], double b[3], double* const presult);
void matrixAdd4x1and4x1(double a[4], double b[4], double* const presult);
void matrixMult3x1byScalar(double vec[3], double scalar,
                           double* const presult);
void matrixMult4x1byScalar(double vec[4], double scalar,
                           double* const presult);
void matrixMult3x3byScalar(double matrix[3][3], double scalar,
                           double* const presult);
void matrixMult3x1by3x3(double bigMatrix[3][3], double littleMatrix[3],
                        double* const presult);
void matrixMult4x4by4x1(double big[4][4], double little[4],
                        double* const presult);
void matrixMult3x3by3x3(double a[3][3], double b[3][3], double* presult);
void crossProduct3x1by3x1(double a[3], double b[3], double * presult);

void eulerAxisToQuaternion(double e[3], double phi, double* const pq);
void quaternionToEulerAxis(double q[4], double* pphi, double* pe);
void findErrorQuaternion(double qc[4], double q[4], double * pqe);
void rotationMatrixFromQuaternion(double q[4], double * pRot);
void quaternionFromEulerAxis(double e[3], double et, double * pQ);
void quaternionCrossProduct(double q1[4], double q2[4], double * pQ);

double getDistance(double a[3], double b[3]);
void getQuaternionErrorMatrix(double q[4], double * pMatrix);
void quaternionFromDCM(double DCM[3][3], double * pQ);

void rotationRoll(double phi, double * pMatrix);
void rotationPitch(double theta, double * pMatrix);
void rotationYaw(double psi, double * pMatrix);

} // namespace math
} // namespace ram
    
#endif // RAM_MATH_H_07_07_2007
