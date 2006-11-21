/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2006 Torus Knot Software Ltd
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
#ifndef __ShadowCameraSetup_H__
#define __ShadowCameraSetup_H__

#include "OgrePrerequisites.h"
#include "OgreMovablePlane.h"
#include "OgreSharedPtr.h"


namespace Ogre {

    /** Used to implement custom algorithms for generating shadow mapping matrices.  
        @remarks
            Shadow map matrices, being projective matrices, have 15 degrees of freedom.
			3 of these degrees of freedom are fixed by the light's position.  4 are used to
			affinely affect z values.  6 affinely affect u,v sampling.  2 are projective
			degrees of freedom.  This class is meant to allow custom methods for 
			handling optimization.
    */
	class _OgreExport ShadowCameraSetup
	{
	public:
		/// Function to implement -- must set the shadow camera properties
		virtual void getShadowCamera (const SceneManager *sm, const Camera *cam, 
									  const Viewport *vp, const Light *light, Camera *texCam) const = 0;
		/// Need virtual destructor in case subclasses use it
		virtual ~ShadowCameraSetup() {}

	};



	/** Implements default shadow camera setup
        @remarks
            This implements the default shadow camera setup algorithm.  This is what might
			be referred to as "normal" shadow mapping.  
    */
	class _OgreExport DefaultShadowCameraSetup : public ShadowCameraSetup
	{
	public:
		/// Default constructor
		DefaultShadowCameraSetup();
		/// Destructor
		virtual ~DefaultShadowCameraSetup();

		/// Default shadow camera setup
		virtual void getShadowCamera (const SceneManager *sm, const Camera *cam, 
									  const Viewport *vp, const Light *light, Camera *texCam) const;
	};



	/** Implements the plane optimal shadow camera algorithm.
        @remarks
            Given a plane of interest, it is possible to set up the shadow camera
			matrix such that the mapping between screen and shadow map is the identity
			(when restricted to pixels that view the plane of interest).  Therefore,
			if the shadow map resolution matches the screen space resolution (of the 
			seen planar receiver), we can get pixel perfect shadowing on the plane. 
			Off the plane, the shadowing is not guaranteed to be perfect and will 
			likely exhibit the usual sampling artifacts associated with shadow mapping.
		@note Important: this routine requires double-precision calculations. When you
			are running under Direct3D, you must ensure that you set the floating
			point mode to 'Consistent' rather than 'Fastest' to ensure this precision.
			This does allegedly come with some performance cost but when measuring 
			it appears to be negligible in modern systems for normal usage.
    */
	class _OgreExport PlaneOptimalShadowCameraSetup : public ShadowCameraSetup
	{
	private:
		MovablePlane* m_plane;	///< pointer to plane of interest
	private:
		PlaneOptimalShadowCameraSetup() {}	///< Default constructor is private

		/// helper function computing projection matrix given constraints
		Matrix4 computeConstrainedProjection( const Vector4& pinhole, 
											  const std::vector<Vector4>& fpoint, 
											  const std::vector<Vector2>& constraint) const;

	public:
		/// Constructor -- requires a plane of interest
		PlaneOptimalShadowCameraSetup(MovablePlane *plane);
		/// Destructor
		virtual ~PlaneOptimalShadowCameraSetup();

		/// Returns shadow camera configured to get 1-1 homography between screen and shadow map when restricted to plane
		virtual void getShadowCamera (const SceneManager *sm, const Camera *cam, 
									  const Viewport *vp, const Light *light, Camera *texCam) const;
	};

	typedef SharedPtr<ShadowCameraSetup> ShadowCameraSetupPtr;

}

#endif
