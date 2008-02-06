// Copyright 2004 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __vector3_to_be_exported_hpp__
#define __vector3_to_be_exported_hpp__

namespace vector3{

class Vector3
{
public:
    union {
        struct {
            float x, y, z;
        };
        float val[3];
    };

public:
    inline Vector3()
    {
    }

    inline Vector3( const float fX, const float fY, const float fZ )
        : x( fX ), y( fY ), z( fZ )
    {
    }

    inline explicit Vector3( const float afCoordinate[3] )
        : x( afCoordinate[0] ),
        y( afCoordinate[1] ),
        z( afCoordinate[2] )
    {
    }

    inline explicit Vector3( const int afCoordinate[3] )
    {
        x = (float)afCoordinate[0];
        y = (float)afCoordinate[1];
        z = (float)afCoordinate[2];
    }

    inline explicit Vector3( float* const r )
        : x( r[0] ), y( r[1] ), z( r[2] )
    {
    }

    inline explicit Vector3( const float scaler )
        : x( scaler )
        , y( scaler )
        , z( scaler )
    {
    }


    inline Vector3( const Vector3& rkVector )
        : x( rkVector.x ), y( rkVector.y ), z( rkVector.z )
    {
    }

    inline float operator [] ( const unsigned int i ) const
    {
        return *(&x+i);
    }

    inline float& operator [] ( const unsigned int i )
    {
        return *(&x+i);
    }

    /** Assigns the value of the other vector.
        @param
            rkVector The other vector
    */
    inline Vector3& operator = ( const Vector3& rkVector )
    {
        x = rkVector.x;
        y = rkVector.y;
        z = rkVector.z;

        return *this;
    }

    inline Vector3& operator = ( const float fScaler )
    {
        x = fScaler;
        y = fScaler;
        z = fScaler;

        return *this;
    }

    inline bool operator == ( const Vector3& rkVector ) const
    {
        return ( x == rkVector.x && y == rkVector.y && z == rkVector.z );
    }

    inline bool operator != ( const Vector3& rkVector ) const
    {
        return ( x != rkVector.x || y != rkVector.y || z != rkVector.z );
    }

    // arithmetic operations
    inline Vector3 operator + ( const Vector3& rkVector ) const
    {
        Vector3 kSum;

        kSum.x = x + rkVector.x;
        kSum.y = y + rkVector.y;
        kSum.z = z + rkVector.z;

        return kSum;
    }

    inline Vector3 operator - ( const Vector3& rkVector ) const
    {
        Vector3 kDiff;

        kDiff.x = x - rkVector.x;
        kDiff.y = y - rkVector.y;
        kDiff.z = z - rkVector.z;

        return kDiff;
    }

    inline Vector3 operator * ( const float fScalar ) const
    {
        Vector3 kProd;

        kProd.x = fScalar*x;
        kProd.y = fScalar*y;
        kProd.z = fScalar*z;

        return kProd;
    }

    inline Vector3 operator * ( const Vector3& rhs) const
    {
        Vector3 kProd;

        kProd.x = rhs.x * x;
        kProd.y = rhs.y * y;
        kProd.z = rhs.z * z;

        return kProd;
    }

    inline Vector3 operator / ( const float fScalar ) const
    {
        Vector3 kDiv;

        float fInv = 1.0 / fScalar;
        kDiv.x = x * fInv;
        kDiv.y = y * fInv;
        kDiv.z = z * fInv;

        return kDiv;
    }

    inline Vector3 operator / ( const Vector3& rhs) const
    {
        Vector3 kDiv;

        kDiv.x = x / rhs.x;
        kDiv.y = y / rhs.y;
        kDiv.z = z / rhs.z;

        return kDiv;
    }


    inline Vector3 operator - () const
    {
        Vector3 kNeg;

        kNeg.x = -x;
        kNeg.y = -y;
        kNeg.z = -z;

        return kNeg;
    }

    // overloaded operators to help Vector3
    inline friend Vector3 operator * ( const float fScalar, const Vector3& rkVector )
    {
        Vector3 kProd;

        kProd.x = fScalar * rkVector.x;
        kProd.y = fScalar * rkVector.y;
        kProd.z = fScalar * rkVector.z;

        return kProd;
    }

    inline friend Vector3 operator + (const Vector3& lhs, const float rhs)
    {
        Vector3 ret(rhs);
        return ret += lhs;
    }

    inline friend Vector3 operator + (const float lhs, const Vector3& rhs)
    {
        Vector3 ret(lhs);
        return ret += rhs;
    }

    inline friend Vector3 operator - (const Vector3& lhs, const float rhs)
    {
        return lhs - Vector3(rhs);
    }

    inline friend Vector3 operator - (const float lhs, const Vector3& rhs)
    {
        Vector3 ret(lhs);
        return ret -= rhs;
    }

    // arithmetic updates
    inline Vector3& operator += ( const Vector3& rkVector )
    {
        x += rkVector.x;
        y += rkVector.y;
        z += rkVector.z;

        return *this;
    }

    inline Vector3& operator += ( const float fScalar )
    {
        x += fScalar;
        y += fScalar;
        z += fScalar;
        return *this;
    }

    inline Vector3& operator -= ( const Vector3& rkVector )
    {
        x -= rkVector.x;
        y -= rkVector.y;
        z -= rkVector.z;

        return *this;
    }

    inline Vector3& operator -= ( const float fScalar )
    {
        x -= fScalar;
        y -= fScalar;
        z -= fScalar;
        return *this;
    }

    inline Vector3& operator *= ( const float fScalar )
    {
        x *= fScalar;
        y *= fScalar;
        z *= fScalar;
        return *this;
    }

    inline Vector3& operator *= ( const Vector3& rkVector )
    {
        x *= rkVector.x;
        y *= rkVector.y;
        z *= rkVector.z;

        return *this;
    }

    inline Vector3& operator /= ( const float fScalar )
    {
        float fInv = 1.0 / fScalar;

        x *= fInv;
        y *= fInv;
        z *= fInv;

        return *this;
    }

    inline Vector3& operator /= ( const Vector3& rkVector )
    {
        x /= rkVector.x;
        y /= rkVector.y;
        z /= rkVector.z;

        return *this;
    }

    inline float squaredLength () const
    {
        return x * x + y * y + z * z;
    }

    Vector3 do_smth( const Vector3& x = ZERO ){
        return x;
    }
    // special points
    static const Vector3 ZERO;

};


}

#endif//__class_order_to_be_exported_hpp__