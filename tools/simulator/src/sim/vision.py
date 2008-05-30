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

import ram.core as core
import ram.timer
from ram.sim.object import IObject
from ram.sim.graphics import IVisual, Visual
from ram.sim.serialization import IKMLStorable, two_step_init


class IBuoy(IObject):
    """ An object which you can see in the simulation"""
    pass

class IPipe(IObject):
    """ An object which you can see in the simulation"""
    pass

class IBin(IObject):
    """ An object which you can see in the simulation"""
    pass

# TODO: Fill out the methods for the class

class Buoy(Visual):
    core.implements(IVisual, IBuoy)
    
    @two_step_init
    def __init__(self):
        Visual.__init__(self)

    def load(self, data_object):
        scene, parent, node = data_object
        Visual.load(self, (scene, parent, node))
        
    def save(self, data_object):
        raise "Not yet implemented"
    
class Pipe(Visual):
    core.implements(IVisual, IPipe)
    
    @two_step_init
    def __init__(self):
        Visual.__init__(self)

    def load(self, data_object):
        scene, parent, node = data_object
        Visual.load(self, (scene, parent, node))
        
    def save(self, data_object):
        raise "Not yet implemented"

class Bin(Visual):
    core.implements(IVisual, IBin)
    
    @two_step_init
    def __init__(self):
        Visual.__init__(self)

    def load(self, data_object):
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
        
        # Red light detector variables
        self._runRedLight = False
        self._foundLight = False
        
        # Orange pipe detector variables
        self._runOrangePipe = False
        self._foundPipe = False
        self._pipeCentered = False
        
        # Bin detector variables
        self._runBin = False
        self._foundBin = False
        self._binCentered = False
        
        # Find all the Buoys, Pipes and Bins
        self._bouys = sim.scene.getObjectsByInterface(IBuoy)
        self._pipes = sim.scene.getObjectsByInterface(IPipe)
        self._bins = sim.scene.getObjectsByInterface(IBin)

    def redLightDetectorOn(self):
        self._runRedLight = True
    
    def redLightDetectorOff(self):
        self._runRedLight = False
        
    def pipeLineDetectorOn(self):
        self._runOrangePipe = True
        
    def pipeLineDetectorOff(self):
        self._runOrangePipe = False
        
    def binDetectorOn(self):
        self._runBin = True
        
    def binDetectorOff(self):
        self._runBin = False

    def backgrounded(self):
        return False

    def update(self, timeSinceLastUpdate):
        """
        Checks against the obstacles, and generates the proper events
        """
        if self._runRedLight:
            self._checkRedLight()
        if self._runOrangePipe:
            self._checkOrangePipe()
        if self._runBin:
            self._checkBin()
    
    def _findClosest(self, objects):
        """
        Find the closest object to the given position
        
        @note: This is inefficient but the object count is to remain low, so
               we are fine.
        """
        closest = None
        obj = None
        relativePos = None
        
        for o in objects:
            toObj = o.position - self.vehicle.robot.position
            
            # None yet found, default to this one
            if closest is None:
               closest = (o, toObj)
            else:
                obj, realtivePos = closest
                if toObj.squaredLength() < realtivePos.squaredLength():
                    # Found a better one switch to it
                    closest = (o, toObj)

        return closest
    
    
    def _checkRedLight(self):
        """
        Check for the red light
        """
        # Determine orientation to the bouy
        lightVisible = False
        bouy, relativePos = self._findClosest(self._bouys)
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
        
    def _downwardCheck(self, relativePos, obj):
        """
        Determines the whether or the object at the given relative position
        is is visible, and if so, it returns its x, y (camera frame cordinates)
        and angle.
        
        @type  relativePos: ogre.renderer.OGRE.Vector3
        @param relativePos: The relative position to the downward object
        
        @rtype:  (bool, double, double, ext.math.Degree)
        @return: (visible, x, y, angle)
        """
        
        camVector = self.vehicle.robot.orientation * -ogre.Vector3.UNIT_Z
        camVector.normalise()

        # Find Roll (X cordinate)
        forwardVector = self.vehicle.robot.orientation * ogre.Vector3.UNIT_X
        rollPlane = ogre.Plane(forwardVector, 0) 
        rollVec = rollPlane.projectVector(relativePos).normalisedCopy()
        roll = ogre.Math.ACos(rollVec.dotProduct(camVector)).valueDegrees()
        
        # Find Pitch (Y cordinate)
        rightVector = self.vehicle.robot.orientation * ogre.Vector3.UNIT_Y
        pitchPlane = ogre.Plane(rightVector, 0) 
        pitchVec = pitchPlane.projectVector(relativePos).normalisedCopy()
        pitch = ogre.Math.ACos(pitchVec.dotProduct(camVector)).valueDegrees()

        # Add in sign
        relativePos = self.vehicle.robot.orientation.UnitInverse() * relativePos
        if relativePos.y < 0:
            roll *= -1
        if relativePos.x < 0:
            pitch *= -1
        
        # Check to see if its the field of view
        visible = False
        x = 0.0
        y = 0.0
        angle = ext.math.Degree(0.0)
        
        if (math.fabs(roll) <= (self._horizontalFOV/2)) and \
           (math.fabs(pitch) <= (self._verticalFOV/2)):
            visible = True
            
            x = roll / (self._horizontalFOV/2) * -1
            y = pitch / (self._verticalFOV/2)
            
            # Find pipe relative angle
            forwardPipe = obj.orientation * ogre.Vector3.UNIT_X
            orientation = forwardVector.getRotationTo(forwardPipe)
            angle = ext.math.Degree(orientation.getRoll(True).valueDegrees())
            
        return (visible, x, y, angle)
        
    def _checkOrangePipe(self):
        pipe, relativePos = self._findClosest(self._pipes)
        pipeVisible, x, y, angle = self._downwardCheck(relativePos, pipe)

        if pipeVisible and (relativePos.length() < 4.5):
            event = ext.core.Event()
            event.x = x
            event.y = y
            event.angle = angle
            self.publish(ext.vision.EventType.PIPE_FOUND, event)
            
            # Check for centering
            toCenter = ogre.Vector2(x, y)
            if toCenter.normalise() < 0.08:
                if not self._pipeCentered:
                    self._pipeCentered = True
                    self.publish(ext.vision.EventType.PIPE_CENTERED, event)
            else:
                self._pipeCentered = False
            
        else:
            if self._foundPipe:
                self.publish(ext.vision.EventType.PIPE_LOST, ext.core.Event())

        self._foundPipe = pipeVisible
        
    def _checkBin(self):
        bin, relativePos = self._findClosest(self._bins)
        binVisible, x, y, angle = self._downwardCheck(relativePos, bin)

        if binVisible and (relativePos.length() < 4.5):
            event = ext.core.Event()
            event.x = x
            event.y = y
            self.publish(ext.vision.EventType.BIN_FOUND, event)
            
            # Check for centering
            toCenter = ogre.Vector2(x, y)
            if toCenter.normalise() < 0.08:
                if not self._binCentered:
                    self._binCentered = True
                    self.publish(ext.vision.EventType.BIN_CENTERED, event)
            else:
                self._binCentered = False
            
        else:
            if self._foundBin:
                self.publish(ext.vision.EventType.BIN_LOST, ext.core.Event())

        self._foundBin = binVisible

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
        downwardOgreCamera = self._createCamera('_downward', (0.5, 0, -0.1),
                                               (0, 0, -1))
        
        # Create _forwardCamera, _forwardBuffer, and _forwardTexture
        self._setupCameraRendering(forwardOgreCamera, 640, 480)
        self._setupCameraRendering(downwardOgreCamera, 640, 480) 

        # Transform arguments to create base VisionSystem class
        cfg = ext.core.ConfigNode.fromString(str(config))
        depList = ext.core.SubsystemList()
        for subsys in deps:
            depList.append(subsys)
        
        ext.vision.VisionSystem.__init__(self, self._forwardCamera,
                                         self._downwardCamera,
                                         cfg, depList)


        cameraRate = config.get('cameraRate', 10)
        self._cameraUpdateInterval = 1.0 / cameraRate

        # Setup render target listeners to do the copying of images
        self._forwardCameraListener = RenderCameraListener(
            self._forwardCamera, self._forwardBuffer, self._forwardTexture,
            self._cameraUpdateInterval)
        self._forwardTexture.getBuffer().getRenderTarget().addListener(
            self._forwardCameraListener)
        
        self._downwardCameraListener = RenderCameraListener(
            self._downwardCamera, self._downwardBuffer, self._downwardTexture,
            self._cameraUpdateInterval)
        self._downwardTexture.getBuffer().getRenderTarget().addListener(
            self._downwardCameraListener)
        
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
        camera.lookAt(camera.position + ogre.Vector3(direction))
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
