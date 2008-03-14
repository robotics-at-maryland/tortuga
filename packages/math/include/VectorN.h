/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Chris Giles <cgiles@umd.edu>
 * All rights reserved.
 *
 * Author: Chris Giles <cgiles@umd.edu>
 * File:  packages/math/include/VectorN.h
 */

#ifndef RAM_MATH_VECTORN_H_13_03_2008
#define RAM_MATH_VECTORN_H_13_03_2008 

// Project Includes
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Vector4.h"
#include "math/include/MatrixN.h"

// Slight hack to allow easier folding in of changes from Ogre
#define Real double
#define OGRE_FORCE_ANGLE_TYPES

// Must Be Included last
#include "math/include/Export.h"


namespace ram {
namespace math {

    /** N-dimensional homogenous vector.
    */
    class RAM_EXPORT VectorN
    {
    protected:
        Real *m_data;
        int m_size;

    public:

        inline VectorN(int size = 1)
        {
            m_size = size;
            m_data = new Real[m_size];
            memset(m_data, 0, sizeof(Real)*m_size);
        }

        inline VectorN(const VectorN& copy)
        {
            m_data = 0;
            m_size = 0;
            *this = copy;
        }

        inline VectorN( const Real* data, int size)
        {
            m_size = size;
            m_data = new Real[m_size];
            memcpy(m_data, data, sizeof(Real) * m_size);
        }

        inline VectorN(const Real scalar, int size)
        {
            m_size = size;
            m_data = new Real[m_size];
            for (int i=0;i<m_size;i++)
                m_data[i] = scalar;
        }

        inline VectorN(const Vector2& v)
        {
            m_size = 2;
            m_data = new Real[m_size];
            m_data[0] = v.x;
            m_data[1] = v.y;
        }

        inline VectorN(const Vector3& v)
        {
            m_size = 3;
            m_data = new Real[m_size];
            m_data[0] = v.x;
            m_data[1] = v.y;
            m_data[2] = v.z;
        }

        inline VectorN(const Vector4& v)
        {
            m_size = 4;
            m_data = new Real[m_size];
            m_data[0] = v.x;
            m_data[1] = v.y;
            m_data[2] = v.z;
            m_data[3] = v.w;
        }

        inline VectorN operator+ (const VectorN& o) const
        {
            assert(m_size == o.m_size);
            VectorN out = *this;
            for (int i=0;i<m_size;i++)
                out[i] += o[i];
            return out;
        }

        inline VectorN operator- (const VectorN& o) const
        {
            assert(m_size == o.m_size);
            VectorN out = *this;
            for (int i=0;i<m_size;i++)
                out[i] -= o[i];
            return out;
        }

        inline Real operator [] ( const size_t i ) const
        {
            assert( (int)i < m_size );
            return m_data[i];
        }

		inline Real& operator [] ( const size_t i )
        {
            assert( (int)i < m_size );
            return m_data[i];
        }

        inline VectorN& operator= (const VectorN& o)
	    {
		    // Handle assign to self
		    if (this == &o)
			    return *this;
		    if (m_size != o.m_size)
		    {
			    delete[] m_data;
			    m_data = new Real[o.m_size];
		    }
                    
            m_size = o.m_size;
            memcpy(m_data, o.m_data, sizeof(Real) * o.m_size);
		    return *this;
	    }

        inline VectorN& operator = ( const Real fScaler )
        {
            for (int i=0;i<m_size;i++)
                m_data[i] = fScaler;

            return *this;
        }

        inline VectorN operator * ( const Real fScalar ) const
        {
            VectorN out = *this;
            for (int i=0;i<m_size;i++)
                out[i] *= fScalar;
            return out;
        }

        inline VectorN operator * ( const VectorN& rhs) const
        {
            assert(m_size == rhs.m_size);
            VectorN out = *this;
            for (int i=0;i<m_size;i++)
                out[i] *= rhs[i];
            return out;
        }

        inline VectorN operator / ( const Real fScalar ) const
        {
            assert( fScalar != 0.0 );
            Real fInv = 1.0 / fScalar;

            VectorN out = *this;
            for (int i=0;i<m_size;i++)
                out[i] *= fInv;
            return out;
        }

        inline VectorN operator / ( const VectorN& rhs) const
        {
            assert(m_size == rhs.m_size);
            VectorN out = *this;
            for (int i=0;i<m_size;i++)
                out[i] /= rhs[i];
            return out;
        }

        inline const VectorN& operator + () const
        {
            return *this;
        }

        inline VectorN operator - () const
        {
            VectorN out = *this;
            for (int i=0;i<m_size;i++)
                out[i] *= -1;
            return out;
        }

        inline bool operator == ( const VectorN& o ) const
        {
            if (m_size != o.m_size)
                return false;
            for (int i=0;i<m_size;i++)
            {
                if (m_data[i] != o.m_data[i])
                    return false;
            }
            return true;
        }

        inline bool operator != ( const VectorN& o ) const
        {
            if (m_size == o.m_size)
                return false;
            for (int i=0;i<m_size;i++)
            {
                if (m_data[i] == o.m_data[i])
                    return false;
            }
            return true;
        }

		inline Real* ptr()
		{
			return m_data;
		}

		inline const Real* ptr() const
		{
			return m_data;
		}


        // overloaded operators to help VectorN
        inline friend VectorN operator * ( const Real fScalar, const VectorN& rkVector )
        {
            return VectorN(rkVector) * fScalar;
        }

        inline friend VectorN operator / ( const Real fScalar, const VectorN& rkVector )
        {
            VectorN out(rkVector.m_size);
            for (int i=0;i<out.m_size;i++)
                out[i] = fScalar / rkVector[i];
            return out;
        }

        inline friend VectorN operator + (const VectorN& lhs, const Real rhs)
        {
            VectorN out(lhs.m_size);
            for (int i=0;i<out.m_size;i++)
                out[i] = rhs + lhs[i];
            return out;
        }

        inline friend VectorN operator + (const Real lhs, const VectorN& rhs)
        {
            VectorN out(rhs.m_size);
            for (int i=0;i<out.m_size;i++)
                out[i] = lhs + rhs[i];
            return out;
        }

        inline friend VectorN operator - (const VectorN& lhs, const Real rhs)
        {
            VectorN out(lhs.m_size);
            for (int i=0;i<out.m_size;i++)
                out[i] = lhs[i] - rhs;
            return out;
        }

        inline friend VectorN operator - (const Real lhs, const VectorN& rhs)
        {
            VectorN out(rhs.m_size);
            for (int i=0;i<out.m_size;i++)
                out[i] = lhs - rhs[i];
            return out;
        }

        // arithmetic updates
        inline VectorN& operator += ( const VectorN& rkVector )
        {
            assert(m_size == rkVector.m_size);
            for (int i=0;i<m_size;i++)
                m_data[i] += rkVector[i];
            return *this;
        }

        inline VectorN& operator += ( const Real fScalar )
        {
            for (int i=0;i<m_size;i++)
                m_data[i] += fScalar;
            return *this;
        }

        inline VectorN& operator -= ( const VectorN& rkVector )
        {
            assert(m_size == rkVector.m_size);
            for (int i=0;i<m_size;i++)
                m_data[i] -= rkVector[i];
            return *this;
        }

        inline VectorN& operator -= ( const Real fScalar )
        {
            for (int i=0;i<m_size;i++)
                m_data[i] -= fScalar;
            return *this;
        }

        inline VectorN& operator *= ( const Real fScalar )
        {
            for (int i=0;i<m_size;i++)
                m_data[i] *= fScalar;
            return *this;
        }

        inline VectorN& operator *= ( const VectorN& rkVector )
        {
            assert(m_size == rkVector.m_size);
            for (int i=0;i<m_size;i++)
                m_data[i] *= rkVector[i];
            return *this;
        }

        inline VectorN& operator /= ( const Real fScalar )
        {
            assert( fScalar != 0.0 );
            Real fInv = 1.0 / fScalar;
            for (int i=0;i<m_size;i++)
                m_data[i] *= fInv;
            return *this;
        }

        inline VectorN& operator /= ( const VectorN& rkVector )
        {
            assert(m_size == rkVector.m_size);
            for (int i=0;i<m_size;i++)
                m_data[i] /= rkVector[i];
            return *this;
        }

        inline Real magnitude() const
        {
            Real t = 0;
            for (int i=0;i<m_size;i++)
                t += m_data[i] * m_data[i];
            return Math::Sqrt(t);
        }

        inline Real magnitudeSquared() const
        {
            Real t = 0;
            for (int i=0;i<m_size;i++)
                t += m_data[i] * m_data[i];
            return t;
        }

        inline Real distance(const VectorN& rhs) const
        {
            assert(m_size == rhs.m_size);
            return (*this - rhs).magnitude();
        }

        inline Real squaredDistance(const VectorN& rhs) const
        {
            assert(m_size == rhs.m_size);
            return (*this - rhs).magnitudeSquared();
        }

        inline Real dotProduct(const VectorN& vec) const
        {
            assert(m_size == vec.m_size);
            Real t  =0;
            for (int i=0;i<m_size;i++)
                t += m_data[i] * vec.m_data[i];
            return t;
        }

        inline Real absDotProduct(const VectorN& vec) const
        {
            assert(m_size == vec.m_size);
            Real t  =0;
            for (int i=0;i<m_size;i++)
                t += Math::Abs(m_data[i] * vec.m_data[i]);
            return t;
        }

        inline Real normalise()
        {
            Real fLength = magnitude();

            // Will also work for zero-sized vectors, but will change nothing
            if ( fLength > 1e-08 )
            {
                Real inv = 1.0f/fLength;
                for (int i=0;i<m_size;i++)
                    m_data[i] *= inv;
            }

            return fLength;
        }

        inline bool operator < ( const VectorN& rhs ) const
        {
            if (m_size < rhs.m_size)
                return true;
            if (m_size > rhs.m_size)
                return false;
            for (int i=0;i<m_size;i++)
                if (m_data[i] > rhs.m_data[i])
                    return false;
            return true;
        }

        inline bool operator > ( const VectorN& rhs ) const
        {
            if (m_size > rhs.m_size)
                return true;
            if (m_size < rhs.m_size)
                return false;
            for (int i=0;i<m_size;i++)
                if (m_data[i] < rhs.m_data[i])
                    return false;
            return true;
        }

        inline bool isZeroLength(void) const
        {
            Real sqlen = magnitudeSquared();
            return (sqlen < (1e-06 * 1e-06));

        }
        inline VectorN normalisedCopy(void) const
        {
            VectorN ret = *this;
            ret.normalise();
            return ret;
        }

        inline bool positionEquals(const VectorN& rhs, Real tolerance = 1e-03) const
		{
            assert(m_size == rhs.m_size);
            for (int i=0;i<m_size;i++)
                if (!Math::RealEqual(m_data[i], rhs.m_data[i], tolerance))
                    return false;
            return true;
		}

        inline int numElements() const
        {
            return m_size;
        }

        inline friend VectorN operator * ( const VectorN& vec, const MatrixN& mat )
        {
            assert(mat.getRows() == vec.numElements());
            VectorN out(mat.getCols());
            for (int i=0;i<out.numElements();i++)
            {
                out[i] = 0;
                for (int j=0;j<vec.numElements();j++)
                {
                    out[i] += mat[j][i] * vec[j];
                }
            }

            return out;
        }

        inline friend VectorN operator * ( const MatrixN mat, const VectorN& vec )
        {
            assert(mat.getCols() == vec.m_size);
            VectorN out(mat.getRows());
            for (int i=0;i<out.m_size;i++)
            {
                out[i] = 0;
                for (int j=0;j<vec.m_size;j++)
                    out[i] += mat[i][j] * vec[j];
            }

            return out;
        }

        inline friend std::ostream& operator <<
            ( std::ostream& o, const VectorN& v )
        {
            o << "VectorN(" << v.m_data[0];
            for (int i=1;i<v.m_size;i++)
                o << ", " << v.m_data[i];
            o << ")";
            return o;
        }

    };
}
}

#endif