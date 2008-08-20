/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Gland <jgland@ssl.umd.edu>
 * File:  packages/math/src/Helpers.cpp
 */
 
// STD Includes
#include <cmath>

// Project Includes
#include "core/include/Platform.h"
#include "math/include/Helpers.h"

namespace ram {
namespace math {

/*
* normalizes a 3x1 vector
* note - this permanently alters the contents of the vector vec
*/
void normalize3x1(double* pvec){
    double a = *(pvec);
    double b = *(pvec+1);
    double c = *(pvec+2);
    double normal = sqrt(a*a+b*b+c*c);
    if(normal == 0){
        *(pvec) = 0;
        *(pvec+1) = 0;
        *(pvec+2) = 0;
    }
    else{
        *(pvec) = a/normal;
        *(pvec+1) = b/normal;
        *(pvec+2) = c/normal;
    }
}

/*
* normalizes a 4x1 vectore
* note - this permanently alters the contents of the vector vec
*/
void normalize4x1(double* pvec){
    double a = *(pvec);
    double b = *(pvec+1);
    double c = *(pvec+2);
    double d = *(pvec+3);
    double normal = sqrt(a*a+b*b+c*c+d*d);

    if(normal == 0){
        *(pvec) = 0;
        *(pvec+1) = 0;
        *(pvec+2) = 0;
        *(pvec+3) = 1;
    }
    else{
        *(pvec) = a/normal;
        *(pvec+1) = b/normal;
        *(pvec+2) = c/normal;
        *(pvec+3) = d/normal;
    }
}

/*
* finds the magnitude of a 3x1 vector
*
*/
double magnitude3x1(double vec[3]){
    double mag;
    mag = sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
    return mag;
}


/*
* finds the magnitude of a 4x1 vector
*
*/
double magnitude4x1(double vec[4]){
    double mag;
    mag = sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]+vec[3]*vec[3]);
    return mag;
}

/*
* finds the determinant of a 2x2 vector
*
*/
double det2x2(double a[2][2]){
    double det=a[0][0]*a[1][1]-a[0][1]*a[1][0];
    return det;
}

/*
* finds the determinant of a 3x3 vector
*
* USES det2x2 !!!!!!!!
*/
double det3x3(double a[3][3]){
    double det;
    double A[2][2]={{a[1][1],a[1][2]},{a[2][1],a[2][2]}};
    double B[2][2]={{a[1][0],a[1][2]},{a[2][0],a[2][2]}};
    double C[2][2]={{a[1][0],a[1][1]},{a[2][0],a[2][1]}};

    det = a[0][0]*det2x2(A)-a[0][1]*det2x2(B)+a[0][2]*det2x2(C);

    return det;
}

/*
* adds two 3x1 vectors together
*
*/
void matrixAdd3x1and3x1(double a[3], double b[3], double* const presult){
    *(presult) = a[0]+b[0];
    *(presult+1) = a[1]+b[1];
    *(presult+2) = a[2]+b[2];
}

/*
* adds two 4x1 vectors together
*
*/
void matrixAdd4x1and4x1(double a[4], double b[4], double* const presult){
    *(presult) = a[0]+b[0];
    *(presult+1) = a[1]+b[1];
    *(presult+2) = a[2]+b[2];
    *(presult+3) = a[3]+b[3];
}

/*
* multiplies a 3x1 vector with a scalar
*
*/
void matrixMult3x1byScalar(double vec[3], double scalar, double* const presult){
    *(presult) = scalar*vec[0];
    *(presult+1) = scalar*vec[1];
    *(presult+2) = scalar*vec[2];
}

/*
* multiplies a 4x1 vector with a scalar
*
*/
void matrixMult4x1byScalar(double vec[4], double scalar, double* const presult){
    *(presult) = scalar*vec[0];
    *(presult+1) = scalar*vec[1];
    *(presult+2) = scalar*vec[2];
    *(presult+3) = scalar*vec[3];
}

/*
* multiplies a 3x3 matrix with a scalar
*
*/
void matrixMult3x3byScalar(double matrix[3][3], double scalar, double* const presult){
    *(presult) = matrix[0][0]*scalar;
    *(presult+1) = matrix[0][1]*scalar;
    *(presult+2) = matrix[0][2]*scalar;
    *(presult+3) = matrix[1][0]*scalar;
    *(presult+4) = matrix[1][1]*scalar;
    *(presult+5) = matrix[1][2]*scalar;
    *(presult+6) = matrix[2][0]*scalar;
    *(presult+7) = matrix[2][1]*scalar;
    *(presult+8) = matrix[2][2]*scalar;
}

/*
* pre-multiplies a 3x1 matrix by a 3x3  (ie result[3][1]=bigMatrix[3][3]*littleMatrix[3][1])
*
*/
void matrixMult3x1by3x3(double bigMatrix[3][3], double littleMatrix[3], double* const presult){
    *(presult) = bigMatrix[0][0]*littleMatrix[0]+bigMatrix[0][1]*littleMatrix[1]+bigMatrix[0][2]*littleMatrix[2];
    *(presult+1) = bigMatrix[1][0]*littleMatrix[0]+bigMatrix[1][1]*littleMatrix[1]+bigMatrix[1][2]*littleMatrix[2];
    *(presult+2) = bigMatrix[2][0]*littleMatrix[0]+bigMatrix[2][1]*littleMatrix[1]+bigMatrix[2][2]*littleMatrix[2];
}

/*
* multiplies a 3x3 matrix with another 3x3 matrix
* note - result matrix MUST be 3x3!  This function doesn't check for that!
* note - this function takes in the result array as a pointer to the first element of that array
*/
void matrixMult3x3by3x3(double a[3][3], double b[3][3], double* presult){
    /*double prod;
    int i,j,k;
    for(i = 0; i < 3; i++){
        for(j = 0; j < 3; j++){
            prod = 0;
            for(k = 0; k < 3; k++){
                prod = prod + a[i][k]*b[k][j];
            }
            *(presult+3*i+j) = prod;
        }
    }*/
    *(presult) = a[0][0]*b[0][0]+a[0][1]*b[1][0]+a[0][2]*b[2][0];
    *(presult+1) = a[0][0]*b[0][1]+a[0][1]*b[1][1]+a[0][2]*b[2][1];
    *(presult+2) = a[0][0]*b[0][2]+a[0][1]*b[1][2]+a[0][2]*b[2][2];
    *(presult+3) = a[1][0]*b[0][0]+a[1][1]*b[1][0]+a[1][2]*b[2][0];
    *(presult+4) = a[1][0]*b[0][1]+a[1][1]*b[1][1]+a[1][2]*b[2][1];
    *(presult+5) = a[1][0]*b[0][2]+a[1][1]*b[1][2]+a[1][2]*b[2][2];
    *(presult+6) = a[2][0]*b[0][0]+a[2][1]*b[1][0]+a[2][2]*b[2][0];
    *(presult+7) = a[2][0]*b[0][1]+a[2][1]*b[1][1]+a[2][2]*b[2][1];
    *(presult+8) = a[2][0]*b[0][2]+a[2][1]*b[1][2]+a[2][2]*b[2][2];
}

/*
* pre-multiplies a 4x1 matrix by a 4x4  (ie result[4][1]=bigMatrix[4][4]*littleMatrix[4][1])
*
*/
void matrixMult4x4by4x1(double big[4][4], double little[4], double* const presult){
    *(presult) = big[0][0]*little[0]+big[0][1]*little[1]+big[0][2]*little[2]+big[0][3]*little[3];
    *(presult+1) = big[1][0]*little[0]+big[1][1]*little[1]+big[1][2]*little[2]+big[1][3]*little[3];
    *(presult+2) = big[2][0]*little[0]+big[2][1]*little[1]+big[2][2]*little[2]+big[2][3]*little[3];
    *(presult+3) = big[3][0]*little[0]+big[3][1]*little[1]+big[3][2]*little[2]+big[3][3]*little[3];
}

/*
implementation of the vector cross product of a and b  (result = a x b)
*/
void crossProduct3x1by3x1(double a[3], double b[3], double * presult){
    *(presult) = a[1]*b[2]-a[2]*b[1];
    *(presult+1) = a[2]*b[0]-a[0]*b[2];
    *(presult+2) = a[0]*b[1]-a[1]*b[0];
}

/*
* converts an euler axis and an euler angle into a quaternion


q - a quaternion with the parameterization:

                   [q1] = [e1*sin(et/2)]
               q = |q2|   |e2*sin(et/2)|
                   |q3|   |e3*sin(et/2)|
                   [q4]   [  cos(et/2) ]

               where euler axis = [e1,e2,e3] and euler angle = et


*/
void eulerAxisToQuaternion(double e[3], double phi, double* const pq){
    normalize3x1(e);
    double normE = e[0]*e[0]+e[1]*e[1]+e[2]*e[2];
    if(normE==0){
        *(pq) = 0;
        *(pq+1) = 0;
        *(pq+2) = 0;
        *(pq+3) = 1;
    }else{
        *(pq) = e[0]*sin(phi/2);
        *(pq+1) = e[1]*sin(phi/2);
        *(pq+2) = e[2]*sin(phi/2);
        *(pq+3) = cos(phi/2);
    }
}

/*
* converts a quaternionan into an euler axis and an euler angle


q - a quaternion with the parameterization:

                   [q1] = [e1*sin(et/2)]
               q = |q2|   |e2*sin(et/2)|
                   |q3|   |e3*sin(et/2)|
                   [q4]   [  cos(et/2) ]

               where euler axis = [e1,e2,e3] and euler angle = et


*/
void quaternionToEulerAxis(double q[4], double* pphi, double* pe){
    //normalize4x1(q);
    *(pphi)=2*acos(q[3]);
    if((2*acos(q[3]))==0){
        *(pe) = 0;
        *(pe+1) = 0;
        *(pe+2) = 1;
    }else{
        *(pe) = q[0]/sin(*(pphi)/2);
        *(pe+1) = q[1]/sin(*(pphi)/2);
        *(pe+2) = q[2]/sin(*(pphi)/2);
    }
}

/*
* creates the tilde matrix, a matrix interpretation of the cross product
*
*/
void tilde(double w[3], double * const ptilde){
    *(ptilde)=0;
    *(ptilde+1)=-w[2];
    *(ptilde+2)=w[1];
    *(ptilde+3)=w[2];
    *(ptilde+4)=0;
    *(ptilde+5)=-w[0];
    *(ptilde+6)=-w[1];
    *(ptilde+7)=w[0];
    *(ptilde+8)=0;
}

/*
* the sign function
*
* returns 1 if variable is positive
* returns -1 if variable is negative
* returns 0 if variale is zero
*/
double sign(double variable){
    double val;
    if(variable>0){
        val = 1;
    }
    else{
        if(variable<0){
            val = -1;
        }
        else{
            val = 0;
        }
    }
    return val;
}

/*
* creates the OMEGA matrix
* used in the quaternion differential equation:
* q_dot = .5*OMEGA*q
*

q - a quaternion with the parameterization:

                   [q1] = [e1*sin(et/2)]
               q = |q2|   |e2*sin(et/2)|
                   |q3|   |e3*sin(et/2)|
                   [q4]   [  cos(et/2) ]

               where euler axis = [e1,e2,e3] and euler angle = et

*/
void createOmega(double w[3], double * pOMEGA){
    *(pOMEGA) = 0;
    *(pOMEGA+1) = w[2];
    *(pOMEGA+2) = -w[1];
    *(pOMEGA+3) = w[0];
    *(pOMEGA+4) = -w[2];
    *(pOMEGA+5) = 0;
    *(pOMEGA+6) = w[0];
    *(pOMEGA+7) = w[1];
    *(pOMEGA+8) = w[1];
    *(pOMEGA+9) = -w[0];
    *(pOMEGA+10) = 0;
    *(pOMEGA+11) = w[2];
    *(pOMEGA+12) = -w[0];
    *(pOMEGA+13) = -w[1];
    *(pOMEGA+14) = -w[2];
    *(pOMEGA+15) = 0;
}

/*
finds the error quaternion between a commanded quaternion (qc) and an actual quaternion (q)

q - a quaternion with the parameterization:

                   [q1] = [e1*sin(et/2)]
               q = |q2|   |e2*sin(et/2)|
                   |q3|   |e3*sin(et/2)|
                   [q4]   [  cos(et/2) ]

               where euler axis = [e1,e2,e3] and euler angle = et

*/
void findErrorQuaternion(double qc[4], double q[4], double * pqe){
    *(pqe) = qc[3]*q[0]+qc[2]*q[1]-qc[1]*q[2]-qc[0]*q[3];
    *(pqe+1) = -qc[2]*q[0]+qc[3]*q[1]+qc[0]*q[2]-qc[1]*q[3];
    *(pqe+2) = qc[1]*q[0]-qc[0]*q[1]+qc[3]*q[2]-qc[2]*q[3];
    *(pqe+3) = qc[0]*q[0]+qc[1]*q[1]+qc[2]*q[2]+qc[3]*q[3];
}


/*
* creates a rotation matrix given the current quaternion

q - a quaternion with the parameterization:

                   [q1] = [e1*sin(et/2)]
               q = |q2|   |e2*sin(et/2)|
                   |q3|   |e3*sin(et/2)|
                   [q4]   [  cos(et/2) ]

               where euler axis = [e1,e2,e3] and euler angle = et

*/
void rotationMatrixFromQuaternion(double q[4], double * pRot){
    *(pRot) = q[0]*q[0]-q[1]*q[1]-q[2]*q[2]+q[3]*q[3];
    *(pRot+1) = 2*(q[0]*q[1]-q[2]*q[3]);
    *(pRot+2) = 2*(q[0]*q[2]-q[1]*q[3]);
    *(pRot+3) = 2*(q[0]*q[1]+q[2]*q[3]);
    *(pRot+4) = -q[0]*q[0]+q[1]*q[1]-q[2]*q[2]+q[3]*q[3];
    *(pRot+5) = 2*(q[1]*q[2]+q[0]*q[3]);
    *(pRot+6) = 2*(q[0]*q[2]-q[1]*q[3]);
    *(pRot+7) = 2*(q[1]*q[2]-q[0]*q[3]);
    *(pRot+8) = -q[0]*q[0]-q[1]*q[1]+q[2]*q[2]+q[3]*q[3];
}

/*
generates a quaternion given an euler axis & angle representation

q - a quaternion with the parameterization:

                   [q1] = [e1*sin(et/2)]
               q = |q2|   |e2*sin(et/2)|
                   |q3|   |e3*sin(et/2)|
                   [q4]   [  cos(et/2) ]

               where euler axis = [e1,e2,e3] and euler angle = et (radians!)

*/
void quaternionFromEulerAxis(double e[3], double et, double * pQ){
  *(pQ) = e[0]*sin(et/2);
  *(pQ+1) = e[1]*sin(et/2);
  *(pQ+2) = e[2]*sin(et/2);
  *(pQ+3) = cos(et/2);
}

/*
a 4 dimensional cross product used for quaternions.  this has the effect
of rotating the second quaternion (q2) by the first quaternion (q1)

this function implements  pQ = q1 x q2

q - a quaternion with the parameterization:

                   [q1] = [e1*sin(et/2)]
               q = |q2|   |e2*sin(et/2)|
                   |q3|   |e3*sin(et/2)|
                   [q4]   [  cos(et/2) ]

               where euler axis = [e1,e2,e3] and euler angle = et (radians!)

*/
void quaternionCrossProduct(double q1[4], double q2[4],  double * pQ){
  //generate quaternion cross product matrix
  double qRotMatrix[4][4];

  /*  *(qRotMatrix) = q1[3];
  *(qRotMatrix+1) = -q1[2];
  *(qRotMatrix+2) = q1[1];
  *(qRotMatrix+3) = q1[0];

  *(qRotMatrix+4) = q1[2];
  *(qRotMatrix+5) = q1[3];
  *(qRotMatrix+6) = -q1[0];
  *(qRotMatrix+7) = q1[1];

  *(qRotMatrix+8) = -q1[1];
  *(qRotMatrix+9) = q1[0];
  *(qRotMatrix+10) = q1[3];
  *(qRotMatrix+11) = q1[2];
  
  *(qRotMatrix+12) = -q1[0];
  *(qRotMatrix+13) = -q1[1];
  *(qRotMatrix+14) = -q1[2];
  *(qRotMatrix+15) = q1[3];*/

  qRotMatrix[0][0] = q1[3];
  qRotMatrix[0][1] = -q1[2];
  qRotMatrix[0][2] = q1[1];
  qRotMatrix[0][3] = q1[0];

  qRotMatrix[1][0] = q1[2];
  qRotMatrix[1][1] = q1[3];
  qRotMatrix[1][2] = -q1[0];
  qRotMatrix[1][3] = q1[1];

  qRotMatrix[2][0] = -q1[1];
  qRotMatrix[2][1] = q1[0];
  qRotMatrix[2][2] = q1[3];
  qRotMatrix[2][3] = q1[2];
  
  qRotMatrix[3][0] = -q1[0];
  qRotMatrix[3][1] = -q1[1];
  qRotMatrix[3][2] = -q1[2];
  qRotMatrix[3][3] = q1[3];

  //matrix multiply!!!
  matrixMult4x4by4x1(qRotMatrix,q2,pQ);
  //normalize to ensure no computational drift
  // normalize4x1(pQ); Broken ?
}


/*
* inverts a 3x3 matrix
*
*/
void invert3x3(double mat[3][3], double * pInverted){
    double detMat = det3x3(mat);
    double coFactor[2][2]={{mat[1][1],mat[1][2]},{mat[2][1],mat[2][2]}};
    *(pInverted) = det2x2(coFactor)/detMat;
    coFactor[0][0] = mat[0][1];
    coFactor[0][1] = mat[0][2];
    coFactor[1][0] = mat[2][1];
    coFactor[1][1] = mat[2][2];
    *(pInverted+1) = -1*det2x2(coFactor)/detMat;
    coFactor[0][0] = mat[0][1];
    coFactor[0][1] = mat[0][2];
    coFactor[1][0] = mat[1][1];
    coFactor[1][1] = mat[1][2];
    *(pInverted+2) = det2x2(coFactor)/detMat;
    coFactor[0][0] = mat[1][0];
    coFactor[0][1] = mat[1][2];
    coFactor[1][0] = mat[2][0];
    coFactor[1][1] = mat[2][2];
    *(pInverted+3) = -1*det2x2(coFactor)/detMat;
    coFactor[0][0] = mat[0][0];
    coFactor[0][1] = mat[0][2];
    coFactor[1][0] = mat[2][0];
    coFactor[1][1] = mat[2][2];
    *(pInverted+4) = det2x2(coFactor)/detMat;
    coFactor[0][0] = mat[0][0];
    coFactor[0][1] = mat[0][2];
    coFactor[1][0] = mat[1][0];
    coFactor[1][1] = mat[1][2];
    *(pInverted+5) = -1*det2x2(coFactor)/detMat;
    coFactor[0][0] = mat[1][0];
    coFactor[0][1] = mat[1][1];
    coFactor[1][0] = mat[2][0];
    coFactor[1][1] = mat[2][1];
    *(pInverted+6) = det2x2(coFactor)/detMat;
    coFactor[0][0] = mat[0][0];
    coFactor[0][1] = mat[0][1];
    coFactor[1][0] = mat[2][0];
    coFactor[1][1] = mat[2][1];
    *(pInverted+7) = -1*det2x2(coFactor)/detMat;
    coFactor[0][0] = mat[0][0];
    coFactor[0][1] = mat[0][1];
    coFactor[1][0] = mat[1][0];
    coFactor[1][1] = mat[1][1];
    *(pInverted+8) = det2x2(coFactor)/detMat;
}


void matrixSaturation3x1(double * pMatrix, double saturationLevel){
    for(int i=0; i<3; ++i){
        if(*(pMatrix+i)>saturationLevel){
            *(pMatrix+i)=saturationLevel;
        }
        if(*(pMatrix+i)<-saturationLevel){
            *(pMatrix+i)=-saturationLevel;
        }
    }
}


/*
computes euclidean distance between points a and b
*/
double getDistance(double a[3], double b[3]){
    double dist;
    dist = sqrt((a[0]-b[0])*(a[0]-b[0])+(a[1]-b[1])*(a[1]-b[1])+(a[2]-b[2])*(a[2]-b[2]));
    return dist;
}

/*
computes matrix for use in quaternion error equation

q - a quaternion with the parameterization:

                   [q1] = [e1*sin(et/2)]
               q = |q2|   |e2*sin(et/2)|
                   |q3|   |e3*sin(et/2)|
                   [q4]   [  cos(et/2) ]

               where euler axis = [e1,e2,e3] and euler angle = et
*/
void getQuaternionErrorMatrix(double q[4], double * pMatrix){
    *(pMatrix) = q[3];
    *(pMatrix+1) = q[2];
    *(pMatrix+2) = -q[1];
    *(pMatrix+3) = -q[0];

    *(pMatrix+4) = -q[2];
    *(pMatrix+5) = q[3];
    *(pMatrix+6) = q[0];
    *(pMatrix+7) = -q[1];

    *(pMatrix+8) = q[1];
    *(pMatrix+9) = -q[0];
    *(pMatrix+10) = q[3];
    *(pMatrix+11) = -q[2];

    *(pMatrix+12) = q[0];
    *(pMatrix+13) = q[1];
    *(pMatrix+14) = q[2];
    *(pMatrix+15) = q[3];
}

/*
finds quaternion given a direction cosine matrix

q - a quaternion with the parameterization:

                   [q1]   [a]   [e1*sin(et/2)]
               q = |q2| = |b| = |e2*sin(et/2)|
                   |q3|   |c|   |e3*sin(et/2)|
                   [q4]   [d]   [  cos(et/2) ]

               where euler axis = [e1,e2,e3] and euler angle = et
*/
void quaternionFromnCb(double nCb[3][3], double * pQ){

    double a, b, c, d;

    d = 0.5*sqrt(nCb[0][0]+nCb[1][1]+nCb[2][2]+1);
    if (fabs(d) >= 0.125){
        a = 0.25*(nCb[2][1]-nCb[1][2])/d;
        b = 0.25*(nCb[0][2]-nCb[2][0])/d;
        c = 0.25*(nCb[1][0]-nCb[0][1])/d;
    }
    else{
        c = 0.5*sqrt(-nCb[0][0]-nCb[1][1]+nCb[2][2]+1);
        if (fabs(c) >= 0.125){
            a = 0.25*(nCb[0][2]-nCb[2][0])/c;
            b = 0.25*(nCb[1][2]+nCb[2][1])/c;
            d = 0.25*(nCb[1][0]-nCb[0][1])/c;
        }
        else{
            b = 0.5*sqrt(-nCb[0][0]+nCb[1][1]-nCb[2][2]+1);
            if (fabs(b) >= 0.125){
                a = 0.25*(nCb[0][1]+nCb[1][0])/b;
                c = 0.25*(nCb[1][2]+nCb[2][1])/b;
                d = 0.25*(nCb[0][2]-nCb[2][0])/b;
            }
            else{
                a = 0.5*sqrt(nCb[0][0]-nCb[1][1]-nCb[2][2]+1);
                b = 0.25*(nCb[0][1]+nCb[1][0])/a;
                c = 0.25*(nCb[0][2]-nCb[2][0])/a;
                d = 0.25*(nCb[2][1]-nCb[1][2])/a;
            }
        }
    }

    //this makes the quaternion error function go nuts
    /*
    if (d<0){
        a = -a;
        b = -b;
        c = -c;
        d = -d;
	}
*/ // ? Broken

    *(pQ) = a;
    *(pQ+1) = b;
    *(pQ+2) = c;
    *(pQ+3) = d;

}

/*
computes a rotation matrix for a given roll
*/
void rotationRoll(double phi, double * pMatrix){
    *(pMatrix) = 1;
    *(pMatrix+1) = 0;
    *(pMatrix+2) = 0;

    *(pMatrix+3) = 0;
    *(pMatrix+4) = cos(phi);
    *(pMatrix+5) = sin(phi);

    *(pMatrix+6) = 0;
    *(pMatrix+7) = -sin(phi);
    *(pMatrix+8) = cos(phi);
}

/*
computes a rotation matrix for a given pitch
*/
void rotationPitch(double theta, double * pMatrix){
    *(pMatrix) = cos(theta);
    *(pMatrix+1) = 0;
    *(pMatrix+2) = -sin(theta);

    *(pMatrix+3) = 0;
    *(pMatrix+4) = 1;
    *(pMatrix+5) = 0;

    *(pMatrix+6) = sin(theta);
    *(pMatrix+7) = 0;
    *(pMatrix+8) = cos(theta);
}

/*
computes a rotation matrix for a given yaw
*/
void rotationYaw(double psi, double * pMatrix){
    *(pMatrix) = cos(psi);
    *(pMatrix+1) = sin(psi);
    *(pMatrix+2) = 0;

    *(pMatrix+3) = -sin(psi);
    *(pMatrix+4) = cos(psi);
    *(pMatrix+5) = 0;

    *(pMatrix+6) = 0;
    *(pMatrix+7) = 0;
    *(pMatrix+8) = 1;
}

} // namespace math
} // namespace ram
