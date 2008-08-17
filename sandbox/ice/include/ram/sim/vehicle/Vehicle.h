/*
 *  Vehicle.h
 *  ice
 *
 *  Created by Lady 3Jane Marie-France Tessier-Ashpool on 7/24/08.
 *  Copyright 2008 TRX Systems. All rights reserved.
 *
 */

#ifndef _RAM_SIM_VEHICLE_VEHICLE_H
#define _RAM_SIM_VEHICLE_VEHICLE_H

#include <ram.h>

namespace ram {
 namespace sim {
  namespace vehicle {
    class Vehicle : virtual public ::ram::vehicle::IVehicle
      {
      private:
	Ogre::SceneNode* mSceneNode;
	std::string mName;
      public:
	inline Vehicle(const std::string& name, Ogre::SceneNode* parent)
	  : mName(name)
	{
	  Ogre::Entity* entity = parent->getCreator()->createEntity("Robot", "robot.mesh");
	  mSceneNode = parent->getCreator()->createChildSceneNode();
	  mSceneNode->atachObject(entity);
	}
	inline ~Vehicle()
	{
	  // Delete all entities under this object's SceneNode
	  while (mSceneNode->numAttachedObjects() > 0)
	    delete (mSceneNode->detachObject(0));
	  // Delete all child SceneNodes
	  mSceneNode->removeAndDestroyAllChildren();
	  // Delete the SceneNode itself
	  mSceneNode->getParent()->removeAndDestroyChild(mSceneNode);
	}
      };
  }
 }
}

#endif
