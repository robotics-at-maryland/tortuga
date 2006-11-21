/*-------------------------------------------------------------------------
This source file is a part of OGRE
(Object-oriented Graphics Rendering Engine)

For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

This library is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License (LGPL) as 
published by the Free Software Foundation; either version 2.1 of the 
License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public 
License for more details.

You should have received a copy of the GNU Lesser General Public License 
along with this library; if not, write to the Free Software Foundation, 
Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA or go to
http://www.gnu.org/copyleft/lesser.txt

You may alternatively use this source under the terms of a specific version of
the OGRE Unrestricted License provided you have obtained such a license from
Torus Knot Software Ltd.
-------------------------------------------------------------------------*/
#include "OgreStableHeaders.h"
#include "OgreShadowTextureManager.h"
#include "OgreResourceGroupManager.h"
#include "OgreTextureManager.h"
#include "OgreStringConverter.h"

namespace Ogre
{

	//-----------------------------------------------------------------------
	template<> ShadowTextureManager* Singleton<ShadowTextureManager>::ms_Singleton = 0;
	ShadowTextureManager* ShadowTextureManager::getSingletonPtr(void)
	{
		return ms_Singleton;
	}
	ShadowTextureManager& ShadowTextureManager::getSingleton(void)
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}
	//---------------------------------------------------------------------
	ShadowTextureManager::ShadowTextureManager()
		: mCount(0)
	{

	}
	//---------------------------------------------------------------------
	ShadowTextureManager::~ShadowTextureManager()
	{
		clear();
	}
	//---------------------------------------------------------------------
	void ShadowTextureManager::getShadowTextures(const ShadowTextureConfigList& config, 
		ShadowTextureList& listToPopulate)
	{
		listToPopulate.clear();

		std::set<Texture*> usedTextures;

		for (ShadowTextureConfigList::const_iterator c = config.begin(); c != config.end(); ++c)
		{
			const ShadowTextureConfig& config = *c;
			bool found = false;
			for (ShadowTextureList::iterator t = mTextureList.begin(); t != mTextureList.end(); ++t)
			{
				const TexturePtr& tex = *t;
				// Skip if already used this one
				if (usedTextures.find(tex.getPointer()) != usedTextures.end())
					continue;

				if (config.width == tex->getWidth() && config.height == tex->getHeight()
					&& config.format == tex->getFormat())
				{
					// Ok, a match
					listToPopulate.push_back(tex);
					usedTextures.insert(tex.getPointer());
					found = true;
					break;
				}
			}
			if (!found)
			{
				// Create a new texture
				static const String baseName = "Ogre/ShadowTexture";
				String targName = baseName + StringConverter::toString(mCount++);
				TexturePtr shadowTex = TextureManager::getSingleton().createManual(
					targName, 
					ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME, 
					TEX_TYPE_2D, config.width, config.height, 0, config.format, 
					TU_RENDERTARGET);
				// Ensure texture loaded
				shadowTex->load();
				listToPopulate.push_back(shadowTex);
				usedTextures.insert(shadowTex.getPointer());
				mTextureList.push_back(shadowTex);
			}
		}

	}
	//---------------------------------------------------------------------
	void ShadowTextureManager::clearUnused()
	{
		for (ShadowTextureList::iterator i = mTextureList.begin(); i != mTextureList.end(); )
		{
			// Unreferenced if only this reference and the resource system
			// Any cached shadow textures should be re-bound each frame dropping
			// any old references
			if ((*i).useCount() == ResourceGroupManager::RESOURCE_SYSTEM_NUM_REFERENCE_COUNTS + 1)
			{
				TextureManager::getSingleton().remove((*i)->getHandle());
				i = mTextureList.erase(i);
			}
			else
			{
				++i;
			}
		}

	}
	//---------------------------------------------------------------------
	void ShadowTextureManager::clear()
	{
		for (ShadowTextureList::iterator i = mTextureList.begin(); i != mTextureList.end(); ++i)
		{
			TextureManager::getSingleton().remove((*i)->getHandle());
		}
		mTextureList.clear();

	}
	//---------------------------------------------------------------------

}

