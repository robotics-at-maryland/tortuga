/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

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
-----------------------------------------------------------------------------
*/
// NOTE THAT THIS FILE IS BASED ON MATERIAL FROM:

// Magic Software, Inc.
// http://www.geometrictools.com/
// Copyright (c) 2000, All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.geometrictools.com/License/WildMagic3License.pdf

#ifndef RAM_MATH_QUATERNION_H_08_05_2007
#define RAM_MATH_QUATERNION_H_08_05_2007

// STD Includes
#include <cassert>
#include <cstddef>
#include <cstring>
#include <ostream>
#include <iostream>
// Project Includes
#include "math/include/Math.h"

// Slight hack to allow easier folding in of changes from Ogre
#define Real double
#define OGRE_FORCE_ANGLE_TYPES

// Must Be Included last
#include "math/include/Export.h"

namespace ram {
namespace math {

    class Matrix3;
    class Vector3;
#ifndef __GCCXML__
    class Matrix4;
    class Vector4;
    class MatrixN;
#endif // __GCCXML__
    
    /** Implementation of a Quaternion, i.e. a rotation around an axis.
    */
    class RAM_EXPORT Quaternion
    {
    public:
        inline Quaternion (
            Real fX = 0.0, Real fY = 0.0, Real fZ = 0.0, Real fW = 1.0)
		{
			w = fW;
			x = fX;
			y = fY;
			z = fZ;
		}
        inline Quaternion (const Quaternion& rkQ)
		{
			w = rkQ.w;
			x = rkQ.x;
			y = rkQ.y;
			z = rkQ.z;
		}
        /// Construct a quaternion from a rotation matrix
        inline Quaternion(const Matrix3& rot)
        {
            this->FromRotationMatrix(rot);
        }
        /// Construct a quaternion from an angle/axis
        inline Quaternion(const Radian& rfAngle, const Vector3& rkAxis)
        {
            this->FromAngleAxis(rfAngle, rkAxis);
        }
#ifndef OGRE_FORCE_ANGLE_TYPES
        inline Quaternion(const Real& rfAngle, const Vector3& rkAxis)
		{
			this->FromAngleAxis(rfAngle, rkAxis);
		}
#endif//OGRE_FORCE_ANGLE_TYPES
        /// Construct a quaternion from 3 orthonormal local axes
        inline Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
        {
            this->FromAxes(xaxis, yaxis, zaxis);
        }
        /// Construct a quaternion from 3 orthonormal local axes
        inline Quaternion(const Vector3* akAxis)
        {
            this->FromAxes(akAxis);
        }
		/// Construct a quaternion from 4 manual w/x/y/z values
		inline Quaternion(Real* valptr)
		{
			memcpy(&x, valptr, sizeof(Real)*4);
		}

		/// Array accessor operator
		inline Real operator [] ( const size_t i ) const
		{
			assert( i < 4 );

			return *(ptr() + i);
		}

		/// Array accessor operator
		inline Real& operator [] ( const size_t i )
		{
			assert( i < 4 );

			return *(ptr() + i);
		}

		/// Pointer accessor for direct copying
		inline Real* ptr()
		{
			return &x;
		}

		/// Pointer accessor for direct copying
		inline const Real* ptr() const
		{
			return &x;
		}

		void FromRotationMatrix (const Matrix3& kRot);

		/*
		 * This creates a rotation matrix that represents the attitude
		 * of the reference frame with respect to the attitude of the
		 * quaternion.  OGRE's rotation matrix is the transpose of R(q)
		 * as shown in Dr. Sanner's and Joseph Galante's work.
		 */
        void ToRotationMatrix (Matrix3& kRot) const;
        void FromAngleAxis (const Radian& rfAngle, const Vector3& rkAxis);
        void ToAngleAxis (Radian& rfAngle, Vector3& rkAxis) const;
        inline void ToAngleAxis (Degree& dAngle, Vector3& rkAxis) const {
            Radian rAngle;
            ToAngleAxis ( rAngle, rkAxis );
            dAngle = rAngle;
        }
#ifndef OGRE_FORCE_ANGLE_TYPES
        inline void FromAngleAxis (const Real& rfAngle, const Vector3& rkAxis) {
			FromAngleAxis ( Angle(rfAngle), rkAxis );
		}
        inline void ToAngleAxis (Real& rfAngle, Vector3& rkAxis) const {
			Radian r;
			ToAngleAxis ( r, rkAxis );
			rfAngle = r.valueAngleUnits();
		}
#endif//OGRE_FORCE_ANGLE_TYPES
        void FromAxes (const Vector3* akAxis);
        void FromAxes (const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
        void ToAxes (Vector3* akAxis) const;
        void ToAxes (Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;
        /// Get the local x-axis
        Vector3 xAxis(void) const;
        /// Get the local y-axis
        Vector3 yAxis(void) const;
        /// Get the local z-axis
        Vector3 zAxis(void) const;

        inline Quaternion& operator= (const Quaternion& rkQ)
		{
			w = rkQ.w;
			x = rkQ.x;
			y = rkQ.y;
			z = rkQ.z;
			return *this;
		}
        Quaternion operator+ (const Quaternion& rkQ) const;
        Quaternion operator- (const Quaternion& rkQ) const;
        Quaternion operator* (const Quaternion& rkQ) const;
        Quaternion operator* (Real fScalar) const;
         friend Quaternion operator* (Real fScalar,
            const Quaternion& rkQ);
        Quaternion operator- () const;
        inline bool operator== (const Quaternion& rhs) const
		{
			return (rhs.x == x) && (rhs.y == y) &&
				(rhs.z == z) && (rhs.w == w);
		}
        inline bool operator!= (const Quaternion& rhs) const
		{
			return !operator==(rhs);
		}
        // functions of a quaternion
        Real Dot (const Quaternion& rkQ) const;  // dot product
        Real Norm () const;  // squared-length
        /// Normalises this quaternion, and returns the previous length
        Real normalise(void); 
        Quaternion Inverse () const;  // apply to non-zero quaternion
        Quaternion UnitInverse () const;  // apply to unit-length quaternion
	Quaternion Power (Real fScala) const;
        Quaternion Exp () const;
        Quaternion Log () const;
	Vector3 Ln();
        // rotation of a vector by a quaternion
        Vector3 operator* (const Vector3& rkVector) const;

   		/** Calculate the local roll element of this quaternion.
		@param reprojectAxis By default the method returns the 'intuitive' result
			that is, if you projected the local Y of the quaterion onto the X and
			Y axes, the angle between them is returned. If set to false though, the
			result is the actual yaw that will be used to implement the quaternion,
			which is the shortest possible path to get to the same orientation and 
			may involve less axial rotation. 
		*/
		Radian getRoll(bool reprojectAxis = true) const;
   		/** Calculate the local pitch element of this quaternion
		@param reprojectAxis By default the method returns the 'intuitive' result
			that is, if you projected the local Z of the quaterion onto the X and
			Y axes, the angle between them is returned. If set to true though, the
			result is the actual yaw that will be used to implement the quaternion,
			which is the shortest possible path to get to the same orientation and 
			may involve less axial rotation. 
		*/
		Radian getPitch(bool reprojectAxis = true) const;
   		/** Calculate the local yaw element of this quaternion
		@param reprojectAxis By default the method returns the 'intuitive' result
			that is, if you projected the local Z of the quaterion onto the X and
			Z axes, the angle between them is returned. If set to true though, the
			result is the actual yaw that will be used to implement the quaternion,
			which is the shortest possible path to get to the same orientation and 
			may involve less axial rotation. 
		*/
		Radian getYaw(bool reprojectAxis = true) const;		
		/// Equality with tolerance (tolerance is max angle difference)
		bool equals(const Quaternion& rhs, const Radian& tolerance) const;
		
        static Quaternion fromDirectionCosineMatrix(Matrix3& nCb);

	    // spherical linear interpolation
        static Quaternion Slerp (Real fT, const Quaternion& rkP,
            const Quaternion& rkQ, bool shortestPath = false);

        static Quaternion SlerpExtraSpins (Real fT,
            const Quaternion& rkP, const Quaternion& rkQ,
            int iExtraSpins);

        // setup for spherical quadratic interpolation
        static void Intermediate (const Quaternion& rkQ0,
            const Quaternion& rkQ1, const Quaternion& rkQ2,
            Quaternion& rka, Quaternion& rkB);

        // spherical quadratic interpolation
        static Quaternion Squad (Real fT, const Quaternion& rkP,
            const Quaternion& rkA, const Quaternion& rkB,
            const Quaternion& rkQ, bool shortestPath = false);

        // normalised linear interpolation - faster but less accurate (non-constant rotation velocity)
        static Quaternion nlerp(Real fT, const Quaternion& rkP, 
            const Quaternion& rkQ, bool shortestPath = false);

        // cutoff for sine near zero
        static const Real ms_fEpsilon;

        // special values
        static const Quaternion ZERO;
        static const Quaternion IDENTITY;

		Real x, y, z, w;

        /** Function for writing to a stream. Outputs "Quaternion(w, x, y, z)" with w,x,y,z
            being the member values of the quaternion.
        */
        inline friend std::ostream& operator <<
            ( std::ostream& o, const Quaternion& q )
        {
            o << "Quaternion(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
            return o;
        }

        /** Find the error between this quaternion and the other quaternion.
         *
         *  This function is written formally as:
         *    q_tilde = q1 (x) q2^-1 where (x) indicates the tensor product
         *  usage:
         *    q_tilde = q1.errorQuaternion(q2);
         *  The q_tilde quaternion defines the rotation from the frame defined
         *  by q2 to the frame defined by q1.
         *
         */
        Quaternion errorQuaternion(Quaternion other);

        /** Compute the derivative of a quaternion based on kinematic relationship to angular velocity */
        Quaternion derivative(Vector3 velocity);

#ifndef __GCCXML__
        /** Create a Q matrix (used for q_dot=0.5*Q(q)*w */
	void toQ(MatrixN* result);
#endif // __GCCXML__

        
    };


} // namespace math
} // namespace ram

// Removal of "Real" hack
#undef Real

#endif // RAM_MATH_QUATERNION_H_08_05_2007
