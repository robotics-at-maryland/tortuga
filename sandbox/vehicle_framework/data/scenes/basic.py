# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>

import Ogre, OgreNewt

import utility
from vehicle.sim.physics import gravityAndBouyancyCallback

class Scene(object):
    def __del__(self):
        del self.bodies
    
    def create_scene(self, graphics_sys, physics_sys):
        """
        This is the main entry point to the scene, it will be called when 
        loading the simulation
        """
        self.scene_mgr = graphics_sys.scene_manager
        self.world = physics_sys.world
        self.bodies = []
        
        # position camera
        graphics_sys.camera.setPosition(0.0, 5.0, 10.0)
        graphics_sys.camera_node.setPosition( 0.0, -3.0, 23.0)
        
        self.create_tank()
        
        # Sky Box.
        self.scene_mgr.setSkyBox(True, "Examples/CloudyNoonSkyBox")
        
        size = Ogre.Vector3(3,1.0,1.0)
        pos = Ogre.Vector3(0,1,0)

        box = utility.create_box(self.scene_mgr, self.world, 
                                 gravityAndBouyancyCallback,
                                 pos = pos, size = size)
        self.bodies.append(box)
        
        # Light
        light = self.scene_mgr.createLight("Light1")
        light.setType(Ogre.Light.LT_POINT )
        light.setPosition(Ogre.Vector3(0.0, 100.0, 100.0))
        
    def create_tank(self):
        # Floor
        floor = self.scene_mgr.createEntity("Floor", "simple_terrain.mesh")
        floornode = \
            self.scene_mgr.getRootSceneNode().createChildSceneNode("FloorNode")
        floornode.attachObject(floor)
        floor.setMaterialName("Simple/BeachStones")
        
        col = OgreNewt.TreeCollision(self.world, floornode, True)
        bod = OgreNewt.Body(self.world, col)
        del col
        bod.attachToNode(floornode)
        bod.setPositionOrientation(Ogre.Vector3(0.0,-10.0,0.0), 
                                   Ogre.Quaternion.IDENTITY )
        self.bodies.append(bod)
        
        # Far Wall
        self.bodies.append(utility.create_plane(self.scene_mgr,self.world,80,20, 
                                                pos = Ogre.Vector3(0,-5,-40)))
        
        # Rear Wall
        self.bodies.append(utility.create_plane(self.scene_mgr,self.world,80,20, 
                                                pos = Ogre.Vector3(0,-5,40),
                                                norm = Ogre.Vector3.UNIT_Z*-1.0))
        
        #self.bodies.append(utility.create_plane(self.scene_mgr,self.world,80,20, 
        #                                        pos = Ogre.Vector3(40,-5,0),
        #                                        norm = Ogre.Vector3.UNIT_X))
        
        self.bodies.append(utility.create_plane(self.scene_mgr,self.world,80,20, 
                                                pos = Ogre.Vector3(-40,-5,0),
                                                norm = Ogre.Vector3.UNIT_X*-1.0))