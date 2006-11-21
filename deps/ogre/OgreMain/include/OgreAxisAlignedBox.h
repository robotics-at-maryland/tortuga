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
#ifndef __AxisAlignedBox_H_
#define __AxisAlignedBox_H_

// Precompiler options
#include "OgrePrerequisites.h"

#include "OgreVector3.h"
#include "OgreMatrix4.h"

namespace Ogre {

    /** A 3D box aligned with the x/y/z axes.
	    @remarks
		    This class represents a simple box which is aligned with the
		    axes. Internally it only stores 2 points as the extremeties of
		    the box, one which is the minima of all 3 axes, and the other
		    which is the maxima of all 3 axes. This class is typically used
		    for an axis-aligned bounding box (AABB) for collision and
		    visibility determination.
    */
    class _OgreExport AxisAlignedBox
    {
    protected:
        enum Extent
        {
            EXTENT_NULL,
            EXTENT_FINITE,
            EXTENT_INFINITE
        };

	    Vector3 mMinimum;
	    Vector3 mMaximum;
	    Extent mExtent;

	    Vector3 mCorners[8];

	    /** Internal method for updating corner points.
	    */
	    void updateCorners(void)
	    {
		    // The order of these items is, using right-handed co-ordinates:
		    // Minimum Z face, starting with Min(all), then anticlockwise
		    //   around face (looking onto the face)
		    // Maximum Z face, starting with Max(all), then anticlockwise
		    //   around face (looking onto the face)
		    mCorners[0] = mMinimum;
		    mCorners[1].x = mMinimum.x; mCorners[1].y = mMaximum.y; mCorners[1].z = mMinimum.z;
		    mCorners[2].x = mMaximum.x; mCorners[2].y = mMaximum.y; mCorners[2].z = mMinimum.z;
		    mCorners[3].x = mMaximum.x; mCorners[3].y = mMinimum.y; mCorners[3].z = mMinimum.z;            

		    mCorners[4] = mMaximum;
		    mCorners[5].x = mMinimum.x; mCorners[5].y = mMaximum.y; mCorners[5].z = mMaximum.z;
		    mCorners[6].x = mMinimum.x; mCorners[6].y = mMinimum.y; mCorners[6].z = mMaximum.z;
		    mCorners[7].x = mMaximum.x; mCorners[7].y = mMinimum.y; mCorners[7].z = mMaximum.z;            
	    }        

    public:
	    inline AxisAlignedBox()
	    {
		    // Default to null box
		    setMinimum( -0.5, -0.5, -0.5 );
		    setMaximum( 0.5, 0.5, 0.5 );
		    mExtent = EXTENT_NULL;
	    }

	    inline AxisAlignedBox( const Vector3& min, const Vector3& max )
	    {
		    setExtents( min, max );
	    }

	    inline AxisAlignedBox(
		    Real mx, Real my, Real mz,
		    Real Mx, Real My, Real Mz )
	    {
		    setExtents( mx, my, mz, Mx, My, Mz );
	    }

	    /** Gets the minimum corner of the box.
	    */
	    inline const Vector3& getMinimum(void) const
	    { 
		    return mMinimum; 
	    }

	    /** Gets the maximum corner of the box.
	    */
	    inline const Vector3& getMaximum(void) const
	    { 
		    return mMaximum;
	    }

	    /** Sets the minimum corner of the box.
	    */
	    inline void setMinimum( const Vector3& vec )
	    {
		    mExtent = EXTENT_FINITE;
		    mMinimum = vec;
		    updateCorners();
	    }

	    inline void setMinimum( Real x, Real y, Real z )
	    {
		    mExtent = EXTENT_FINITE;
		    mMinimum.x = x;
		    mMinimum.y = y;
		    mMinimum.z = z;
		    updateCorners();
	    }

	    /** Sets the maximum corner of the box.
	    */
	    inline void setMaximum( const Vector3& vec )
	    {
		    mExtent = EXTENT_FINITE;
		    mMaximum = vec;
		    updateCorners();
	    }

	    inline void setMaximum( Real x, Real y, Real z )
	    {
		    mExtent = EXTENT_FINITE;
		    mMaximum.x = x;
		    mMaximum.y = y;
		    mMaximum.z = z;
		    updateCorners();
	    }

	    /** Sets both minimum and maximum extents at once.
	    */
	    inline void setExtents( const Vector3& min, const Vector3& max )
	    {
		    mExtent = EXTENT_FINITE;
		    mMinimum = min;
		    mMaximum = max;
		    updateCorners();
	    }

	    inline void setExtents(
		    Real mx, Real my, Real mz,
		    Real Mx, Real My, Real Mz )
	    {
		    mExtent = EXTENT_FINITE;

		    mMinimum.x = mx;
		    mMinimum.y = my;
		    mMinimum.z = mz;

		    mMaximum.x = Mx;
		    mMaximum.y = My;
		    mMaximum.z = Mz;

		    updateCorners();
	    }

	    /** Returns a pointer to an array of 8 corner points, useful for
		    collision vs. non-aligned objects.
		    @remarks
			    If the order of these corners is important, they are as
			    follows: The 4 points of the minimum Z face (note that
			    because Ogre uses right-handed coordinates, the minimum Z is
			    at the 'back' of the box) starting with the minimum point of
			    all, then anticlockwise around this face (if you are looking
			    onto the face from outside the box). Then the 4 points of the
			    maximum Z face, starting with maximum point of all, then
			    anticlockwise around this face (looking onto the face from
			    outside the box). Like this:
			    <pre>
			       1-----2
			      /|    /|
			     / |   / |
			    5-----4  |
			    |  0--|--3
			    | /   | /
			    |/    |/
			    6-----7
			    </pre>
	    */
	    inline const Vector3* getAllCorners(void) const
	    {
		    assert( (mExtent == EXTENT_FINITE) && "Can't get corners of a null or infinite AAB" );
		    return (const Vector3*)mCorners;
	    }

	    friend std::ostream& operator<<( std::ostream& o, AxisAlignedBox aab )
	    {
            switch (aab.mExtent)
            {
            case EXTENT_NULL:
			    o << "AxisAlignedBox(null)";
                return o;

            case EXTENT_FINITE:
			    o << "AxisAlignedBox(min=" << aab.mMinimum << ", max=" << aab.mMaximum;
			    o << ", corners=";
			    for (int i = 0; i < 7; ++i)
				    o << aab.mCorners[i] << ", ";
			    o << aab.mCorners[7] << ")";
                return o;

            case EXTENT_INFINITE:
			    o << "AxisAlignedBox(infinite)";
                return o;

            default: // shut up compiler
                assert( false && "Never reached" );
                return o;
            }
	    }

	    /** Merges the passed in box into the current box. The result is the
		    box which encompasses both.
	    */
	    void merge( const AxisAlignedBox& rhs )
	    {
		    // Do nothing if rhs null, or this is infinite
		    if ((rhs.mExtent == EXTENT_NULL) || (mExtent == EXTENT_INFINITE))
		    {
			    return;
		    }
            // Otherwise if rhs is infinite, make this infinite, too
            else if (rhs.mExtent == EXTENT_INFINITE)
            {
                mExtent = EXTENT_INFINITE;
            }
		    // Otherwise if current null, just take rhs
		    else if (mExtent == EXTENT_NULL)
		    {
			    *this = rhs;
		    }
		    // Otherwise merge
		    else
		    {
			    Vector3 min = mMinimum;
			    Vector3 max = mMaximum;
			    max.makeCeil(rhs.mMaximum);
			    min.makeFloor(rhs.mMinimum);

			    setExtents(min, max);
		    }

	    }
		
		/** Extends the box to encompass the specified point (if needed).
		*/
		void merge( const Vector3& point )
		{
            switch (mExtent)
            {
            case EXTENT_NULL: // if null, use this point
				setExtents(point, point);
                return;

            case EXTENT_FINITE:
				mMaximum.makeCeil(point);
				mMinimum.makeFloor(point);
				updateCorners();
                return;

            case EXTENT_INFINITE: // if infinite, makes no difference
                return;
            }

            assert( false && "Never reached" );
		}

	    /** Transforms the box according to the matrix supplied.
		    @remarks
			    By calling this method you get the axis-aligned box which
			    surrounds the transformed version of this box. Therefore each
			    corner of the box is transformed by the matrix, then the
			    extents are mapped back onto the axes to produce another
			    AABB. Useful when you have a local AABB for an object which
			    is then transformed.
	    */
	    void transform( const Matrix4& matrix )
	    {
		    // Do nothing if current null or infinite
		    if( mExtent != EXTENT_FINITE )
			    return;

		    Vector3 min, max, temp;
		    bool first = true;
		    size_t i;

		    for( i = 0; i < 8; ++i )
		    {
			    // Transform and check extents
			    temp = matrix * mCorners[i];
			    if( first || temp.x > max.x )
				    max.x = temp.x;
			    if( first || temp.y > max.y )
				    max.y = temp.y;
			    if( first || temp.z > max.z )
				    max.z = temp.z;
			    if( first || temp.x < min.x )
				    min.x = temp.x;
			    if( first || temp.y < min.y )
				    min.y = temp.y;
			    if( first || temp.z < min.z )
				    min.z = temp.z;

			    first = false;
		    }

		    setExtents( min,max );

	    }

	    /** Transforms the box according to the affine matrix supplied.
		    @remarks
			    By calling this method you get the axis-aligned box which
			    surrounds the transformed version of this box. Therefore each
			    corner of the box is transformed by the matrix, then the
			    extents are mapped back onto the axes to produce another
			    AABB. Useful when you have a local AABB for an object which
			    is then transformed.
            @note
                The matrix must be an affine matrix. @see Matrix4::isAffine.
	    */
	    void transformAffine(const Matrix4& m)
	    {
            assert(m.isAffine());

		    // Do nothing if current null or infinite
		    if ( mExtent != EXTENT_FINITE )
			    return;

            Vector3 centre = getCenter();
            Vector3 halfSize = getHalfSize();

            Vector3 newCentre = m.transformAffine(centre);
            Vector3 newHalfSize(
                Math::Abs(m[0][0]) * halfSize.x + Math::Abs(m[0][1]) * halfSize.y + Math::Abs(m[0][2]) * halfSize.z, 
                Math::Abs(m[1][0]) * halfSize.x + Math::Abs(m[1][1]) * halfSize.y + Math::Abs(m[1][2]) * halfSize.z,
                Math::Abs(m[2][0]) * halfSize.x + Math::Abs(m[2][1]) * halfSize.y + Math::Abs(m[2][2]) * halfSize.z);

            setExtents(newCentre - newHalfSize, newCentre + newHalfSize);
        }

	    /** Sets the box to a 'null' value i.e. not a box.
	    */
	    inline void setNull()
	    {
		    mExtent = EXTENT_NULL;
	    }

	    /** Returns true if the box is null i.e. empty.
	    */
	    bool isNull(void) const
	    {
		    return (mExtent == EXTENT_NULL);
	    }

        /** Returns true if the box is finite.
        */
        bool isFinite(void) const
        {
            return (mExtent == EXTENT_FINITE);
        }

        /** Sets the box to 'infinite'
        */
        inline void setInfinite()
        {
            mExtent = EXTENT_INFINITE;
        }

        /** Returns true if the box is infinite.
        */
        bool isInfinite(void) const
        {
            return (mExtent == EXTENT_INFINITE);
        }

        /** Returns whether or not this box intersects another. */
        inline bool intersects(const AxisAlignedBox& b2) const
        {
            // Early-fail for nulls
            if (this->isNull() || b2.isNull())
                return false;

            // Early-success for infinites
            if (this->isInfinite() || b2.isInfinite())
                return true;

            // Use up to 6 separating planes
            if (mMaximum.x < b2.mMinimum.x)
                return false;
            if (mMaximum.y < b2.mMinimum.y)
                return false;
            if (mMaximum.z < b2.mMinimum.z)
                return false;

            if (mMinimum.x > b2.mMaximum.x)
                return false;
            if (mMinimum.y > b2.mMaximum.y)
                return false;
            if (mMinimum.z > b2.mMaximum.z)
                return false;

            // otherwise, must be intersecting
            return true;

        }

		/// Calculate the area of intersection of this box and another
		inline AxisAlignedBox intersection(const AxisAlignedBox& b2) const
		{
			if (!this->intersects(b2))
			{
				return AxisAlignedBox();
			}
            else if (this->isInfinite())
            {
                return b2;
            }
            else if (b2.isInfinite())
            {
                return *this;
            }

			Vector3 intMin, intMax;

			const Vector3& b2max = b2.getMaximum();
			const Vector3& b2min = b2.getMinimum();

			if (b2max.x > mMaximum.x && mMaximum.x > b2min.x)
				intMax.x = mMaximum.x;
			else 
				intMax.x = b2max.x;
			if (b2max.y > mMaximum.y && mMaximum.y > b2min.y)
				intMax.y = mMaximum.y;
			else 
				intMax.y = b2max.y;
			if (b2max.z > mMaximum.z && mMaximum.z > b2min.z)
				intMax.z = mMaximum.z;
			else 
				intMax.z = b2max.z;

			if (b2min.x < mMinimum.x && mMinimum.x < b2max.x)
				intMin.x = mMinimum.x;
			else
				intMin.x= b2min.x;
			if (b2min.y < mMinimum.y && mMinimum.y < b2max.y)
				intMin.y = mMinimum.y;
			else
				intMin.y= b2min.y;
			if (b2min.z < mMinimum.z && mMinimum.z < b2max.z)
				intMin.z = mMinimum.z;
			else
				intMin.z= b2min.z;

			return AxisAlignedBox(intMin, intMax);

		}

		/// Calculate the volume of this box
		Real volume(void) const
		{
            switch (mExtent)
            {
            case EXTENT_NULL:
				return 0.0f;

            case EXTENT_FINITE:
                {
                    Vector3 diff = mMaximum - mMinimum;
                    return diff.x * diff.y * diff.z;
                }

            case EXTENT_INFINITE:
                return Math::POS_INFINITY;

            default: // shut up compiler
                assert( false && "Never reached" );
                return 0.0f;
            }
		}

        /** Scales the AABB by the vector given. */
        inline void scale(const Vector3& s)
        {
		    // Do nothing if current null or infinite
            if (mExtent != EXTENT_FINITE)
                return;

            // NB assumes centered on origin
            Vector3 min = mMinimum * s;
            Vector3 max = mMaximum * s;
            setExtents(min, max);
        }

		/** Tests whether this box intersects a sphere. */
		bool intersects(const Sphere& s) const
		{
			return Math::intersects(s, *this); 
		}
		/** Tests whether this box intersects a plane. */
		bool intersects(const Plane& p) const
		{
			return Math::intersects(p, *this);
		}
        /** Tests whether the vector point is within this box. */
        bool intersects(const Vector3& v) const
        {
            switch (mExtent)
            {
            case EXTENT_NULL:
                return false;

            case EXTENT_FINITE:
                return(v.x >= mMinimum.x  &&  v.x <= mMaximum.x  && 
                    v.y >= mMinimum.y  &&  v.y <= mMaximum.y  && 
                    v.z >= mMinimum.z  &&  v.z <= mMaximum.z);

            case EXTENT_INFINITE:
                return true;

            default: // shut up compiler
                assert( false && "Never reached" );
                return false;
            }
        }
		/// Gets the centre of the box
		Vector3 getCenter(void) const
		{
            assert( (mExtent == EXTENT_FINITE) && "Can't get center of a null or infinite AAB" );

		    return Vector3(
		        (mMaximum.x + mMinimum.x) * 0.5,
		        (mMaximum.y + mMinimum.y) * 0.5,
		        (mMaximum.z + mMinimum.z) * 0.5);
		}
		/// Gets the size of the box
		Vector3 getSize(void) const
		{
            switch (mExtent)
            {
            case EXTENT_NULL:
                return Vector3::ZERO;

            case EXTENT_FINITE:
                return Vector3(
                    (mMaximum.x - mMinimum.x),
                    (mMaximum.y - mMinimum.y),
                    (mMaximum.z - mMinimum.z));

            case EXTENT_INFINITE:
                return Vector3(
                    Math::POS_INFINITY,
                    Math::POS_INFINITY,
                    Math::POS_INFINITY);

            default: // shut up compiler
                assert( false && "Never reached" );
                return Vector3::ZERO;
            }
		}
		/// Gets the half-size of the box
		Vector3 getHalfSize(void) const
		{
            switch (mExtent)
            {
            case EXTENT_NULL:
                return Vector3::ZERO;

            case EXTENT_FINITE:
                return Vector3(
                    (mMaximum.x - mMinimum.x) * 0.5,
                    (mMaximum.y - mMinimum.y) * 0.5,
                    (mMaximum.z - mMinimum.z) * 0.5);

            case EXTENT_INFINITE:
                return Vector3(
                    Math::POS_INFINITY,
                    Math::POS_INFINITY,
                    Math::POS_INFINITY);

            default: // shut up compiler
                assert( false && "Never reached" );
                return Vector3::ZERO;
            }
		}


    };

} // namespace Ogre

#endif
