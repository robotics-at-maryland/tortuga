/*
-----------------------------------------------------------------------------
This source file is COPIED from OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

ogre crap:
Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.

You may alternatively use this source under the terms of a specific version of
the OGRE Unrestricted License provided you have obtained such a license from
Torus Knot Software Ltd.


R@M crap:
we basically took the Matrix3 class and rewrote it for a general 2x2 matrix

edited by Joseph Gland and Jaymit Patel on 2008-2-19

-----------------------------------------------------------------------------
*/
#ifndef RAM_MATH_MATRIX2_H_08_05_2007
#define RAM_MATH_MATRIX2_H_08_05_2007

// STD Includes
#include <ostream>

// Project Includes
#include "math/include/Vector2.h"

// NB All code adapted from Wild Magic 0.2 Matrix math (free source code)
// http://www.geometrictools.com/

// NOTE.  The (x,y,z) coordinate system is assumed to be right-handed.
// Coordinate axis rotation matrices are of the form
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the yz-plane
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// where t > 0 indicates a counterclockwise rotation in the zx-plane
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a counterclockwise rotation in the xy-plane.


// Slight hack to allow easier folding in of changes from Ogre
#define Real double
#define OGRE_FORCE_ANGLE_TYPES

// Must Be Included last
#include "math/include/Export.h"

namespace ram {
namespace math {

    /** A 2x2 matrix 
        @note
            <b>All the code is adapted from the Wild Magic 0.2 Matrix
            library (http://www.geometrictools.com/).</b>
    */
    class RAM_EXPORT Matrix2
    {
    public:
        /** Default constructor.
            @note
                It does <b>NOT</b> initialize the matrix for efficiency.
        */
		inline Matrix2 () {};
        inline explicit Matrix2 (const Real arr[2][2])
		{
			memcpy(m,arr,4*sizeof(Real));
		}
        inline Matrix2 (const Matrix2& rkMatrix)
		{
			memcpy(m,rkMatrix.m,4*sizeof(Real));
		}
        Matrix2 (Real fEntry00, Real fEntry01,
                    Real fEntry10, Real fEntry11)
		{
			m[0][0] = fEntry00;
			m[0][1] = fEntry01;
			m[1][0] = fEntry10;
			m[1][1] = fEntry11;
		}

        // member access, allows use of construct mat[r][c]
        inline Real* operator[] (size_t iRow) const
		{
			return (Real*)m[iRow];
		}
        /*inline operator Real* ()
		{
			return (Real*)m[0];
		}*/
        Vector2 GetColumn (size_t iCol) const;
        void SetColumn(size_t iCol, const Vector2& vec);
        //void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis); meaningless for 2d

        // assignment and comparison
        inline Matrix2& operator= (const Matrix2& rkMatrix)
		{
			memcpy(m,rkMatrix.m,4*sizeof(Real));
			return *this;
		}
        bool operator== (const Matrix2& rkMatrix) const;
        inline bool operator!= (const Matrix2& rkMatrix) const
		{
			return !operator==(rkMatrix);
		}

        // arithmetic operations
        Matrix2 operator+ (const Matrix2& rkMatrix) const;
        Matrix2 operator- (const Matrix2& rkMatrix) const;
        Matrix2 operator* (const Matrix2& rkMatrix) const;
        Matrix2 operator- () const;

        // matrix * vector [2x2 * 2x1 = 2x1]
        Vector2 operator* (const Vector2& rkVector) const;

        // vector * matrix [1x2 * 2x2 = 1x2]
         friend Vector2 operator* (const Vector2& rkVector,
            const Matrix2& rkMatrix);

        // matrix * scalar
        Matrix2 operator* (Real fScalar) const;

        // scalar * matrix
         friend Matrix2 operator* (Real fScalar, const Matrix2& rkMatrix);

        // utilities
        /* THIS STUFF is neat but we don't have time to implement
        Matrix2 Transpose () const;
        bool Inverse (Matrix2& rkInverse, Real fTolerance = 1e-06) const;
        Matrix2 Inverse (Real fTolerance = 1e-06) const;
        Real Determinant () const;

        // singular value decomposition
        void SingularValueDecomposition (Matrix2& rkL, Vector2& rkS,
            Matrix2& rkR) const;
        void SingularValueComposition (const Matrix2& rkL,
            const Vector2& rkS, const Matrix2& rkR);

        // Gram-Schmidt orthonormalization (applied to columns of rotation matrix)
        //OGRE people probably didn't write Gram-Schmidt for general matrices, dubious that this will work
        void Orthonormalize ();

        // orthogonal Q, diagonal D, upper triangular U stored as (u01,u02,u12)
        void QDUDecomposition (Matrix2& rkQ, Vector2& rkD,
            Vector2 rkU) const;

        Real SpectralNorm () const;

        */
        
        /*
        // matrix must be orthonormal
        void ToAxisAngle (Vector3& rkAxis, Radian& rfAngle) const;
		inline void ToAxisAngle (Vector3& rkAxis, Degree& rfAngle) const {
			Radian r;
			ToAxisAngle ( rkAxis, r );
			rfAngle = r;
		}
        void FromAxisAngle (const Vector3& rkAxis, const Radian& fRadians);
#ifndef OGRE_FORCE_ANGLE_TYPES
        inline void ToAxisAngle (Vector3& rkAxis, Real& rfRadians) const {
			Radian r;
			ToAxisAngle ( rkAxis, r );
			rfRadians = r.valueRadians();
		}
        inline void FromAxisAngle (const Vector3& rkAxis, Real fRadians) {
			FromAxisAngle ( rkAxis, Radian(fRadians) );
		}
#endif//OGRE_FORCE_ANGLE_TYPES
        */

        // The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
        // where yaw is rotation about the Up vector, pitch is rotation about the
        // Right axis, and roll is rotation about the Direction axis.
        /*
        bool ToEulerAnglesXYZ (Radian& rfYAngle, Radian& rfPAngle,
            Radian& rfRAngle) const;
        bool ToEulerAnglesXZY (Radian& rfYAngle, Radian& rfPAngle,
            Radian& rfRAngle) const;
        bool ToEulerAnglesYXZ (Radian& rfYAngle, Radian& rfPAngle,
            Radian& rfRAngle) const;
        bool ToEulerAnglesYZX (Radian& rfYAngle, Radian& rfPAngle,
            Radian& rfRAngle) const;
        bool ToEulerAnglesZXY (Radian& rfYAngle, Radian& rfPAngle,
            Radian& rfRAngle) const;
        bool ToEulerAnglesZYX (Radian& rfYAngle, Radian& rfPAngle,
            Radian& rfRAngle) const;
        void FromEulerAnglesXYZ (const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
        void FromEulerAnglesXZY (const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
        void FromEulerAnglesYXZ (const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
        void FromEulerAnglesYZX (const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
        void FromEulerAnglesZXY (const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
        void FromEulerAnglesZYX (const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
        */
        /*
#ifndef OGRE_FORCE_ANGLE_TYPES
        inline bool ToEulerAnglesXYZ (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const {
			Radian y, p, r;
			bool b = ToEulerAnglesXYZ(y,p,r);
			rfYAngle = y.valueRadians();
			rfPAngle = p.valueRadians();
			rfRAngle = r.valueRadians();
			return b;
		}
        inline bool ToEulerAnglesXZY (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const {
			Radian y, p, r;
			bool b = ToEulerAnglesXZY(y,p,r);
			rfYAngle = y.valueRadians();
			rfPAngle = p.valueRadians();
			rfRAngle = r.valueRadians();
			return b;
		}
        inline bool ToEulerAnglesYXZ (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const {
			Radian y, p, r;
			bool b = ToEulerAnglesYXZ(y,p,r);
			rfYAngle = y.valueRadians();
			rfPAngle = p.valueRadians();
			rfRAngle = r.valueRadians();
			return b;
		}
        inline bool ToEulerAnglesYZX (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const {
			Radian y, p, r;
			bool b = ToEulerAnglesYZX(y,p,r);
			rfYAngle = y.valueRadians();
			rfPAngle = p.valueRadians();
			rfRAngle = r.valueRadians();
			return b;
		}
        inline bool ToEulerAnglesZXY (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const {
			Radian y, p, r;
			bool b = ToEulerAnglesZXY(y,p,r);
			rfYAngle = y.valueRadians();
			rfPAngle = p.valueRadians();
			rfRAngle = r.valueRadians();
			return b;
		}
        inline bool ToEulerAnglesZYX (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const {
			Radian y, p, r;
			bool b = ToEulerAnglesZYX(y,p,r);
			rfYAngle = y.valueRadians();
			rfPAngle = p.valueRadians();
			rfRAngle = r.valueRadians();
			return b;
		}
        inline void FromEulerAnglesXYZ (float fYAngle, float fPAngle, float fRAngle) {
			FromEulerAnglesXYZ ( Radian(fYAngle), Radian(fPAngle), Radian(fRAngle) );
		}
        inline void FromEulerAnglesXZY (float fYAngle, float fPAngle, float fRAngle) {
			FromEulerAnglesXZY ( Radian(fYAngle), Radian(fPAngle), Radian(fRAngle) );
		}
        inline void FromEulerAnglesYXZ (float fYAngle, float fPAngle, float fRAngle) {
			FromEulerAnglesYXZ ( Radian(fYAngle), Radian(fPAngle), Radian(fRAngle) );
		}
        inline void FromEulerAnglesYZX (float fYAngle, float fPAngle, float fRAngle) {
			FromEulerAnglesYZX ( Radian(fYAngle), Radian(fPAngle), Radian(fRAngle) );
		}
        inline void FromEulerAnglesZXY (float fYAngle, float fPAngle, float fRAngle) {
			FromEulerAnglesZXY ( Radian(fYAngle), Radian(fPAngle), Radian(fRAngle) );
		}
        inline void FromEulerAnglesZYX (float fYAngle, float fPAngle, float fRAngle) {
			FromEulerAnglesZYX ( Radian(fYAngle), Radian(fPAngle), Radian(fRAngle) );
		}
#endif//OGRE_FORCE_ANGLE_TYPES
        */
        // eigensolver, matrix must be symmetric
        //most likely written for 3x3 only
        /*
        void EigenSolveSymmetric (Real afEigenvalue[3],
            Vector3 akEigenvector[3]) const;

        static void TensorProduct (const Vector3& rkU, const Vector3& rkV,
            Matrix3& rkProduct);
            */

            
        
		/** Determines if this matrix involves a scaling. */
        /*
		inline bool hasScale() const
		{
			// check magnitude of column vectors (==local axes)
			Real t = m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0];
			if (!Math::RealEqual(t, 1.0, 1e-04))
				return true;
			t = m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1];
			if (!Math::RealEqual(t, 1.0, 1e-04))
				return true;
			t = m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2];
			if (!Math::RealEqual(t, 1.0, 1e-04))
				return true;

			return false;
		}*/


        static const Real EPSILON;
        static const Matrix2 ZERO;
        static const Matrix2 IDENTITY;

        /** Function for writing to a stream.
        */
        inline  friend std::ostream& operator <<
            ( std::ostream& o, const Matrix2& m )
        {
            o << "Matrix2(";
			for (size_t i = 0; i < 2; ++i)
            {
                o << " row" << (unsigned)i << "{";
                for(size_t j = 0; j < 2; ++j)
                {
                    o << m[i][j] << " ";
                }
                o << "}";
            }
            o << ")";
            return o;
        }
        
    protected:
        // support for eigensolver
        /*
        void Tridiagonal (Real afDiag[3], Real afSubDiag[3]);
        bool QLAlgorithm (Real afDiag[3], Real afSubDiag[3]);
        */
        
        // support for singular value decomposition
        static const Real ms_fSvdEpsilon;
        static const unsigned int ms_iSvdMaxIterations;
        static void Bidiagonalize (Matrix2& kA, Matrix2& kL,
            Matrix2& kR);
        static void GolubKahanStep (Matrix2& kA, Matrix2& kL,
            Matrix2& kR);

        // support for spectral norm
        static Real MaxCubicRoot (Real afCoeff[2]);

        Real m[2][2];

        // for faster access
        //friend class Matrix4; we don't need this (we think....)
    };

} // namespace math
} // namespace ram

// Removal of "Real" hack
#undef Real

#endif // RAM_MATH_MATRIX2_H_08_05_2007
