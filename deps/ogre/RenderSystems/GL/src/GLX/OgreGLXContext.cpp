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

#include "OgreGLRenderSystem.h"
#include "OgreRoot.h"
#include "OgreGLXContext.h"

namespace Ogre {

    GLXContext::GLXContext(::Display *dpy,
                ::GLXDrawable drawable,
                ::GLXContext ctx, 
				::XVisualInfo* visualInfo) :
        mDpy(dpy), mDrawable(drawable), mCtx(ctx), mVisualInfo(visualInfo)
	{
                  
    }
    GLXContext::GLXContext(::Display *dpy,
                ::GLXDrawable drawable,
                ::GLXContext ctx, 
				::GLXFBConfig fbconfig) :
        mDpy(dpy), mDrawable(drawable), mCtx(ctx), mVisualInfo(0), mFBConfig(fbconfig)
	{
                  
    }
    GLXContext::~GLXContext() {
		// Unregister and destroy this context
		// This will disable it if it was still active
		// NB have to do this is subclass to ensure any methods called back
		// are on this subclass and not half-destructed superclass
		GLRenderSystem *rs = static_cast<GLRenderSystem*>(Root::getSingleton().getRenderSystem());
		rs->_unregisterContext(this);
    }

    void GLXContext::setCurrent() {
        glXMakeCurrent(mDpy, mDrawable, mCtx);
    }
	void GLXContext::endCurrent() {
		glXMakeCurrent(mDpy, None, NULL);
	}

	GLContext* GLXContext::clone() const
	{
		// Create a new context, share lists with existing

		if (mVisualInfo) // window context clone
		{
			::GLXContext newCtx = glXCreateContext(mDpy, mVisualInfo, mCtx,True);
			return new GLXContext(mDpy, mDrawable, newCtx, mVisualInfo);
		}
		else // non-window
		{
			::GLXContext newCtx = glXCreateNewContext(mDpy, mFBConfig, GLX_RGBA_TYPE, mCtx, True);
			return new GLXContext(mDpy, mDrawable, newCtx, mFBConfig);
		}      
	} 

}
