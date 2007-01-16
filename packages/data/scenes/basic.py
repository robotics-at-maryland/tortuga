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
        graphics_sys.camera_node.setPosition( 0, 1, 3)
        graphics_sys.camera.lookAt(0, 0, 0)
        
        self.create_tank()
        self.scene_mgr.setSkyBox(True, "Examples/CloudyNoonSkyBox")
        
        # Light
        light = self.scene_mgr.createLight("Light1")
        light.setType(Ogre.Light.LT_POINT )
        light.setPosition(Ogre.Vector3(0.0, 100.0, 100.0))
        
    def add_wall(self, pos, norm = Ogre.Vector3.UNIT_Z, 
                 orient = Ogre.Quaternion.IDENTITY):
        wall_body = utility.create_plane(self.scene_mgr,self.world,80,20, 
                                         pos = pos, norm = norm, 
                                         orient = orient)
        self.bodies.append(wall_body)
        
    def create_tank(self):
        # Floor
        floor = self.scene_mgr.createEntity("Floor", "simple_terrain.mesh")
        floor.setMaterialName("Simple/BeachStones")
        floornode = \
            self.scene_mgr.getRootSceneNode().createChildSceneNode("FloorNode")
        floornode.attachObject(floor)
      
        col = OgreNewt.TreeCollision(self.world, floornode, True)
        bod = OgreNewt.Body(self.world, col)
        del col
        bod.attachToNode(floornode)
        bod.setPositionOrientation(Ogre.Vector3(0.0,-10.0,0.0), 
                                   Ogre.Quaternion.IDENTITY )
        self.bodies.append(bod)
        
        # Far Wall
        self.add_wall(Ogre.Vector3(0,-5,-40))
        # Rear Wall
        self.add_wall(Ogre.Vector3(0,-5,40), norm = Ogre.Vector3.UNIT_Z*-1.0)
        # Right Side Wall
        orient = Ogre.Quaternion( Ogre.Degree(d=-90), Ogre.Vector3(0,1,0))
        self.add_wall(Ogre.Vector3(40,-5,0), orient = orient)
        # Left Side Wall
        orient = Ogre.Quaternion( Ogre.Degree(d=90), Ogre.Vector3(0,1,0))
        self.add_wall(Ogre.Vector3(-40,-5,0), orient = orient)
        
        # Water
        orient = Ogre.Quaternion( Ogre.Degree(d=-90), Ogre.Vector3(1,0,0) ) 
        plane_body = utility.create_plane(self.scene_mgr,self.world,80,80,  
                                          material = "Simple/Translucent", 
                                          orient = orient)