/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://ogre.sourceforge.net/

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
// Ogre includes
#include "OgreStableHeaders.h"

#include "OgreResource.h"
#include "OgreResourceManager.h"
#include "OgreResourceBackgroundQueue.h"
#include "OgreLogManager.h"
#include "OgreException.h"

namespace Ogre 
{
	//-----------------------------------------------------------------------
	Resource::Resource(ResourceManager* creator, const String& name, ResourceHandle handle,
		const String& group, bool isManual, ManualResourceLoader* loader)
		: mCreator(creator), mName(name), mGroup(group), mHandle(handle), 
		mLoadingState(LOADSTATE_UNLOADED), mIsBackgroundLoaded(false),
		mSize(0), mIsManual(isManual), mLoader(loader)
	{
	}
	//-----------------------------------------------------------------------
	Resource::~Resource() 
	{ 
	}
	//-----------------------------------------------------------------------
	void Resource::escalateLoading()
	{
		// Just call load as if this is the background thread, locking on
		// load status will prevent race conditions
		load(true);
	}
	//-----------------------------------------------------------------------
	void Resource::load(bool background)
	{
		// Early-out without lock (mitigate perf cost of ensuring loaded)
		// Don't load if:
		// 1. We're already loaded
		// 2. Another thread is loading right now
		// 3. We're marked for background loading and this is not the background
		//    loading thread we're being called by
		if (mLoadingState != LOADSTATE_UNLOADED || (mIsBackgroundLoaded && !background))
			return;

		// Scope lock over load status
		{
			OGRE_LOCK_MUTEX(mLoadingStatusMutex)
			// Check again just in case status changed (since we didn't lock above)
			if (mLoadingState != LOADSTATE_UNLOADED || (mIsBackgroundLoaded && !background))
			{
				// no loading to be done
				return;
			}
			mLoadingState = LOADSTATE_LOADING;
		}

		// Scope lock for actual loading
        try
		{

			OGRE_LOCK_AUTO_MUTEX
			preLoadImpl();

			if (mIsManual)
			{
				// Load from manual loader
				if (mLoader)
				{
					mLoader->loadResource(this);
				}
				else
				{
					// Warn that this resource is not reloadable
					LogManager::getSingleton().logMessage(
						"WARNING: " + mCreator->getResourceType() + 
						" instance '" + mName + "' was defined as manually "
						"loaded, but no manual loader was provided. This Resource "
						"will be lost if it has to be reloaded.");
				}
			}
			else
			{
				if (mGroup == ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME)
				{
					// Derive resource group
					changeGroupOwnership(
						ResourceGroupManager::getSingleton()
						.findGroupContainingResource(mName));
				}
				loadImpl();
			}
			// Calculate resource size
			mSize = calculateSize();

			postLoadImpl();
		}
        catch (...)
        {
            // Reset loading in-progress flag in case failed for some reason
            OGRE_LOCK_MUTEX(mLoadingStatusMutex)
            mLoadingState = LOADSTATE_UNLOADED;
            // Re-throw
            throw;
        }

		// Scope lock for loading progress
		{
			OGRE_LOCK_MUTEX(mLoadingStatusMutex)
		
			// Now loaded
			mLoadingState = LOADSTATE_LOADED;
		}

		// Notify manager
		if(mCreator)
			mCreator->_notifyResourceLoaded(this);

		// Fire (deferred) events
		if (mIsBackgroundLoaded)
			queueFireBackgroundLoadingComplete();


	}
	//-----------------------------------------------------------------------
	void Resource::changeGroupOwnership(const String& newGroup)
	{
		if (mGroup != newGroup)
		{
			String oldGroup = mGroup;
			mGroup = newGroup;
			ResourceGroupManager::getSingleton()
				._notifyResourceGroupChanged(oldGroup, this);
		}
	}
	//-----------------------------------------------------------------------
	void Resource::unload(void) 
	{ 
		// Early-out without lock (mitigate perf cost of ensuring unloaded)
		if (mLoadingState == LOADSTATE_UNLOADED)
			return;

		// Scope lock for loading status
		{
			OGRE_LOCK_MUTEX(mLoadingStatusMutex)
			if (mLoadingState == LOADSTATE_LOADING)
			{
				OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
					"Cannot unload resource " + mName + " whilst loading is in progress!", 
					"Resource::unload");
			}
			if (mLoadingState != LOADSTATE_LOADED)
				return; // nothing to do
		}

		// Scope lock for actual unload
		{
			OGRE_LOCK_AUTO_MUTEX
			preUnloadImpl();
			unloadImpl();
			postUnloadImpl();
		}

		// Scope lock for loading status
		{
			OGRE_LOCK_MUTEX(mLoadingStatusMutex)
			mLoadingState = LOADSTATE_UNLOADED;
		}

		// Notify manager
		if(mCreator)
			mCreator->_notifyResourceUnloaded(this);

	}
	//-----------------------------------------------------------------------
	void Resource::reload(void) 
	{ 
		OGRE_LOCK_AUTO_MUTEX
		if (mLoadingState == LOADSTATE_LOADED)
		{
			unload();
			load();
		}
	}
	//-----------------------------------------------------------------------
	void Resource::touch(void) 
	{
		OGRE_LOCK_AUTO_MUTEX
        // make sure loaded
        load();

		if(mCreator)
			mCreator->_notifyResourceTouched(this);
	}
	//-----------------------------------------------------------------------
	void Resource::addListener(Resource::Listener* lis)
	{
		mListenerList.push_back(lis);
	}
	//-----------------------------------------------------------------------
	void Resource::removeListener(Resource::Listener* lis)
	{
		// O(n) but not called very often
		mListenerList.remove(lis);
	}
	//-----------------------------------------------------------------------
	void Resource::queueFireBackgroundLoadingComplete(void)
	{
		ResourceBackgroundQueue& rbq = ResourceBackgroundQueue::getSingleton();
		for (ListenerList::iterator i = mListenerList.begin();
			i != mListenerList.end(); ++i)
		{
			rbq._queueFireBackgroundLoadingComplete(*i, this);
		}
	}


}
