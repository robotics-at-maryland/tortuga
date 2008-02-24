# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/simulator/src/sim/vision.py

# STD Imports
import math
import ctypes

# Library Imports
import ogre.renderer.OGRE as ogre

# Project Imports
import ext.core
import ext.vision
import ext.math
from sim.subsystems import Simulation
from sim.vehicle import SimVehicle

import core
import ram.timer
from ram.sim.object import IObject
from ram.sim.graphics import IVisual, Visual
from ram.sim.serialization import IKMLStorable, two_step_init


class IBuoy(IObject):
    """ An object which you can see in the simulation"""
    pass

# TODO: Fill out the methods for the class

class Buoy(Visual):
    core.implements(IVisual, IBuoy)
    
    _plane_count = 0
    
    @two_step_init
    def __init__(self):
        Visual.__init__(self)

#    def init(self, parent, name, scene, mesh, material,
#             position = Ogre.Vector3.ZERO, 
#             orientation = Ogre.Quaternion.IDENTITY,
#             scale = Ogre.Vector3(1,1,1)):
#        Visual.init(self, parent, name, scene, mesh, material, position, 
#                    orientation, scale)

    # IStorable Methods
    def load(self, data_object):
        """
        @type  data_object: tuple
        @param data_object: (scene, parent, kml_node)
        """
        scene, parent, node = data_object
        Visual.load(self, (scene, parent, node))
        
    def save(self, data_object):
        raise "Not yet implemented"

class IdealSimVision(ext.vision.VisionSystem):
    def __init__(self, config, deps):
        # Transform arguments to create base VisionSystem class
        cfg = ext.core.ConfigNode.fromString(str(config))
        depList = ext.core.SubsystemList()
        for subsys in deps:
            depList.append(subsys)

        # The camera do nothing
        ext.vision.VisionSystem.__init__(self, ext.vision.Camera(640, 480),
                                         ext.vision.Camera(640, 480), cfg,
                                         depList)
        
        
        # Grab the scene we are operating in
        sim = ext.core.Subsystem.getSubsystemOfType(Simulation, deps, 
                                                    nonNone = True)
        self.vehicle = ext.core.Subsystem.getSubsystemOfType(SimVehicle, deps, 
                                                             nonNone = True)
        self._horizontalFOV = config.get('horizontalFOV', 107)
        self._verticalFOV = config.get('verticalFOV', 78)
        self._runRedLight = False
        
        # Find all the Buoy's
        self._foundLight = False
        self._bouys = []
        for obj in sim.scene._objects:
            if IBuoy in core.providedBy(obj):
                self._bouys.append(obj)
        
    def redLightDetectorOn(self):
        self._runRedLight = True
    
    def redLightDetectorOff(self):
        self._runRedLight = False

    def update(self, timeSinceLastUpdate):
        """
        Checks against the obstacles, and generates the proper events
        """
        if self._runRedLight:
            self._checkRedLight()

    
    def _checkRedLight(self):
        """
        Check for the red light
        """
        
        # Find the closest bouy
        # Note: This is inefficient but the buoy count is to remain low, so
        # we are fine.
        closest = None
        for b in self._bouys:
            toBuoy = b.position - self.vehicle.robot.position
            
            if closest is None:
               closest = (b, toBuoy)
            else:
                buoy, realtivePos = closest
                if toBuoy.squaredLength() < realtivePos.squaredLength():
                    closest = (b, toBuoy)
                    
        # Determine orientation to the bouy
        lightVisible = False
        bouy, relativePos = closest
        forwardVector = self.vehicle.robot.orientation * ogre.Vector3.UNIT_X
        
        quat = forwardVector.getRotationTo(relativePos)
        yaw = -quat.getRoll(True).valueDegrees()
        pitch = quat.getYaw(True).valueDegrees()
        
        # Check to see if its the field of view
        if (math.fabs(yaw) <= (self._horizontalFOV/2)) and \
           (math.fabs(pitch) <= (self._verticalFOV/2)):
            lightVisible = True
        
        if lightVisible and (relativePos.length() < 3):
            event = ext.core.Event()

            event.x = yaw / (self._horizontalFOV/2)
            # Negative because of the corindate system
            event.y = -pitch / (self._verticalFOV/2)
            
            # These have to be swaped as well
            event.azimuth = ext.math.Degree(-yaw)
            event.elevation = ext.math.Degree(-pitch)
            
            # Convert to feet
            event.range = relativePos.length() * 3.2808399
            
            self.publish(ext.vision.EventType.LIGHT_FOUND, event)

            if relativePos.length() < 0.5:
                self.publish(ext.vision.EventType.LIGHT_ALMOST_HIT,
                             ext.core.Event())
            
        else:
            if self._foundLight:
                self.publish(ext.vision.EventType.LIGHT_LOST, ext.core.Event())

        self._foundLight = lightVisible
        

ext.core.SubsystemMaker.registerSubsystem('IdealSimVision', IdealSimVision)

class RenderCameraListener(ogre.RenderTargetListener):
    """
    Listens for the rendering of our camera textures, copies them to system
    memory, then ejects them into the vision system.
    """
    
    def __init__(self, camera, buffer_, texture, updateInterval):
        """
        @type  camera: ext.vision.Camera
        @param camera: The camera to send the render image to
        
        @type  buffer: ctypes array
        @param buffer: The buffer to copy the renderered image to
        
        @type  texture: ogre.renderer.OGRE.Texture
        @param texture: The texture that it rendered to by Ogre
        """
        
        ogre.RenderTargetListener.__init__(self)
        self._camera = camera
        self._bufferAddress = ctypes.addressof(buffer_)
        self._texture = texture
        self._updateInterval = updateInterval
        
        self._image = None
        self._lastTime = 0

    def postRenderTargetUpdate(self, renderTargetEvent):
        now = ram.timer.time()
        if 0 == self._lastTime:
            timePassed = self._updateInterval
        else:
            timePassed = now - self._lastTime

        # Only update at the desired interval
        if timePassed >= self._updateInterval:
            self._updateCamera(timePassed)

            self._lastTime = now

    def _updateCamera(self, timeSinceLastUpdate):
        # Lock all of the texture buffer
        textureBuffer = self._texture.getBuffer()
        lockBox = ogre.Box(0, 0, 640, 480)
        textureBuffer.lock(lockBox, ogre.HardwareBuffer.HBL_NORMAL)

        # Copy the lock portion our image
        texPb = textureBuffer.getCurrentLock()

        # Convert and copy our pixels
        ogre.PixelUtil.bulkPixelConversion(ogre.CastInt(texPb.getData()),
                                           textureBuffer.getFormat(),
                                           self._bufferAddress,
                                           ogre.PixelFormat.PF_R8G8B8,
                                           640 * 480)

        # Create our temporary image
        self._image = ext.vision.Image.loadFromBuffer(self._bufferAddress,
                                                      640, 480, False)
        # Send image to camera
        self._camera.capturedImage(self._image)
        
        textureBuffer.unlock()

        

class SimVision(ext.vision.VisionSystem):
    def __init__(self, config, deps):
        self.vehicle = ext.core.Subsystem.getSubsystemOfType(SimVehicle, deps, 
                                                             nonNone = True)

        # Creates ogre.renderer.OGRE.Camera and attaches it to the vehicle
        forwardOgreCamera = self._createCamera('_forward', (0.5, 0, 0),
                                               (1, 0, 0))
        # Create _forwardCamera, _forwardBuffer, and _forwardTexture
        self._setupCameraRendering(forwardOgreCamera, 640, 480)

        # Transform arguments to create base VisionSystem class
        cfg = ext.core.ConfigNode.fromString(str(config))
        depList = ext.core.SubsystemList()
        for subsys in deps:
            depList.append(subsys)
        
        ext.vision.VisionSystem.__init__(self, self._forwardCamera,
                                         ext.vision.Camera(640, 480),
                                         cfg, depList)

        cameraRate = config.get('cameraRate', 10)
        self._cameraUpdateInterval = 1.0 / cameraRate

        # Setup render target listeners to do the copying of images
        self._forwardCameraListener = RenderCameraListener(
            self._forwardCamera, self._forwardBuffer, self._forwardTexture,
            self._cameraUpdateInterval)
        self._forwardTexture.getBuffer().getRenderTarget().addListener(
            self._forwardCameraListener)
        
    def _setupCameraRendering(self, camera, width, height):
        """
        @type  camera: ogre.renderer.OGRE.Camera
        @param camera: The Ogre camera to grab images from
        """ 

        ImageBufferType = ctypes.c_uint8 * (640 * 480 * 3)

        # Create our camera to allow communication with the vision system
        setattr(self, camera.name + 'Camera', ext.vision.Camera(width, height))
        # Buffer we copy the rendered image into
        setattr(self, camera.name + 'Buffer', ImageBufferType())

        # Create our texture which the camera renderers to
        renderSys = ogre.Root.getSingleton().getRenderSystem()
        texture = ogre.TextureManager.getSingleton().createManual(
            camera.name + 'RenderTexture', 
            ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,
            ogre.TEX_TYPE_2D, width, height, 0, ogre.PixelFormat.PF_B8G8R8,
            ogre.TU_RENDERTARGET)
        rttTex = texture.getBuffer().getRenderTarget();
        rttTex.addViewport(camera)

        setattr(self, camera.name + 'Texture', texture)

    def _createCamera(self, name, position, direction):
        """Lets hack on a camera (integrate better in the future)"""
        
        node = self.vehicle.robot._main_part._node
        sceneMgr = ogre.Root.getSingleton().getSceneManager('Main')

        # Create camera and attached it to our ourself
        camera = sceneMgr.createCamera(name)

        # Align and Position
        camera.position = position
        camera.lookAt(ogre.Vector3(direction).normalisedCopy())
        camera.nearClipDistance = 0.05
        node.attachObject(camera)

        # This needs be set from the config file (only VERTICAL FOV)
        camera.FOVy = ogre.Degree(78)

        # NOTE: Fix not needed because camera on the vehicle is offset in the
        #       same way, what an odd coincidence
        # Account for the odd up vector difference between our and Ogre's 
        # default coordinate systems
        #camera.roll(ogre.Degree(-90))

        return camera

    def saveForwardCameraSnapshot(self, filename):
        """
        Saves the current forward camera image to file
        """
        address = ctypes.addressof(self._forwardBuffer)
        image = ext.vision.Image.loadFromBuffer(address, 640, 480, False)
        ext.vision.Image.saveToFile(image, filename)


ext.core.SubsystemMaker.registerSubsystem('SimVision', SimVision)
