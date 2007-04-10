# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>

import ogre.renderer.OGRE as Ogre
import ogre.physics.OgreNewt as OgreNewt

import utility
from sim.util import gravityAndBouyancyCallback

# These globals are a hack!
bodies = []
scene_mgr = None
world = None

def create_scene(loader, scene):
    """
    This is the main entry point to the scene, it will be called when 
    loading the simulation
    """
    global scene_mgr
    global world
    global bodies
    scene_mgr = scene._scene_manager
    world = scene._world
    
    locations = {'Zip': ['../media/packs/OgreCore.zip', 
                         '../media/packs/cubemapsJS.zip'],
                 'FileSystem' : ['../media/models', '../media/primitives',
                                  '../media/materials/textures', 
                                  '../media/materials/scripts'] }
    
    #print dir(scene)
    scene.add_resource_locations(locations)
    
    # Create Camera here
    camera = scene_mgr.createCamera('PlayerCam')
    camera.setPosition(Ogre.Vector3( 0.0, 5.0, 5.0))
    camera.lookAt(Ogre.Vector3(0, 0, 0))
    camera.nearClipDistance = 0.5
                
    # Allows easier movement of camera
    camera_node = scene_mgr.getRootSceneNode().createChildSceneNode()
    camera_node.setPosition(0,0,0)
    camera_node.attachObject(camera)
    
    # position camera
    camera_node.setPosition( 0, 1, 3)
    camera.lookAt(0, 0, 0)
    
    # TODO: Remove this hack, place functionality in Scene class
    scene.cameras = {}
    scene.cameras['Main'] = camera
    
    create_tank()
    scene_mgr.setSkyBox(True, "Examples/CloudyNoonSkyBox")

    
    # Light
    scene_mgr.setAmbientLight(Ogre.ColourValue(1,1,1))
    light = scene_mgr.createLight("Light1")
    light.setType(Ogre.Light.LT_POINT )
    light.setPosition(Ogre.Vector3(0.0, 100.0, 100.0))
    return bodies

def add_wall(pos, norm = Ogre.Vector3.UNIT_Z, 
             orient = Ogre.Quaternion.IDENTITY):
    global scene_mgr
    global world
    global bodies
    
    
    wall_body = utility.create_plane(scene_mgr,world,80,20, 
                                     pos = pos, norm = norm, 
                                     orient = orient)
    bodies.append(wall_body)
    
def create_tank():
    global scene_mgr
    global world
    global bodies
    
    # Floor
    floor = scene_mgr.createEntity("Floor", "simple_terrain.mesh")
    floor.setMaterialName("Simple/BeachStones")
    floornode = \
        scene_mgr.getRootSceneNode().createChildSceneNode("FloorNode")
    floornode.attachObject(floor)

    col = OgreNewt.TreeCollision(world, floornode, True)
    bod = OgreNewt.Body(world, col)
    del col
    bod.attachToNode(floornode)
    bod.setPositionOrientation(Ogre.Vector3(0.0,-10.0,0.0), 
                               Ogre.Quaternion.IDENTITY )
    bodies.append(bod)
    
    # Far Wall
    add_wall(Ogre.Vector3(0,-5,-40))
    # Rear Wall
    add_wall(Ogre.Vector3(0,-5,40), norm = Ogre.Vector3.UNIT_Z*-1.0)
    # Right Side Wall
    orient = Ogre.Quaternion( Ogre.Degree(d=-90), Ogre.Vector3(0,1,0))
    add_wall(Ogre.Vector3(40,-5,0), orient = orient)
    # Left Side Wall
    orient = Ogre.Quaternion( Ogre.Degree(d=90), Ogre.Vector3(0,1,0))
    add_wall(Ogre.Vector3(-40,-5,0), orient = orient)
    
    # Water
    orient = Ogre.Quaternion( Ogre.Degree(d=-90), Ogre.Vector3(1,0,0) ) 
    plane_body = utility.create_plane(scene_mgr,world,80,80,  
                                      material = "Simple/Translucent", 
                                      orient = orient)