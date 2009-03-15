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
from ext.vision import Suit
from sim.subsystems import Simulation
from sim.vehicle import SimVehicle

import ram.sim
import ram.core as core
import ram.timer
from ram.sim.object import IObject
from ram.sim.graphics import IVisual, Visual
from ram.sim.serialization import two_step_init
from ram.sim.serialization import parse_position_orientation
import ram.sim.object

class IBuoy(IObject):
    """ An object which you can see in the simulation"""

class IPipe(IObject):
    """ An object which you can see in the simulation"""
    pass

class IBarbedWire(IObject):
    """ An object which you can see in the simulation"""
    pass

class IBin(IObject):
    """ An object which you can see in the simulation"""
    pass

class ITarget(IObject):
    """ An object which you can see in the simulation"""
    pass

class IDuct(IObject):
    """ An object which you can see in the simulation"""
    pass

class ISafe(IObject):
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
        if not node.has_key('Graphical'):
            # Default mesh and scale info
            gfxNode = {'mesh' : 'sphere.50cm.mesh', 
                       'scale' : [0.15, 0.15, 0.15],
                       'material' : 'Simple/Red' }
            node['Graphical'] = gfxNode
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
        if not node.has_key('Graphical'):
            # Default mesh and scale info
            gfxNode = {'mesh' : 'cube.1m.mesh', 
                       'scale' : [1.2192, 0.1524, 0.0254],
                       'material' : 'Simple/Orange' }
            node['Graphical'] = gfxNode
        Visual.load(self, (scene, parent, node))
        
    def save(self, data_object):
        raise "Not yet implemented"
    
class BarbedWire(ram.sim.object.Object):
    core.implements(ram.sim.object.IObject, IBarbedWire)
    
    SEPERATION = 1.2192
    
    @two_step_init
    def __init__(self):
        ram.sim.object.Object.__init__(self)
        self._front = None
        self._back = None
    
    def _toAxisAngleArray(self, orientation):
        angle = ogre.Degree(0)
        vector = ogre.Vector3()
        orientation.ToAngleAxis(angle, vector)
        return [vector.x, vector.y, vector.z, angle.valueDegrees()]
    
    @property
    def position(self):
        return self._position
    
    @property
    def orientation(self):
        return self._orientation
    
    def load(self, data_object):
        scene, parent, node = data_object
        ram.sim.object.Object.load(self, (parent, node))
        
        # Parse config information
        basepos, orientation = parse_position_orientation(node)
        self._position = ram.sim.OgreVector3(basepos)
        self._orientation = orientation
        basePos = ram.sim.OgreVector3(basepos)
        baseName = node['name']
        
        frontBackOffset = \
            orientation * ogre.Vector3(BarbedWire.SEPERATION/2, 0, 0)
        drawOrientation = orientation * ogre.Quaternion(
            ogre.Degree(90), ogre.Vector3.UNIT_Z)
        
        # Shared graphics node
        gfxNode = {'mesh': 'cylinder.mesh', 
                   'material' : 'Simple/Green',
                   'scale': [1.8288, 0.0508, 0.0508] }
        
        # Create Front Pipe
        position = basePos + (frontBackOffset * -1)
        cfg = {'name' : baseName + 'BarbedWireFront', 
               'position' : position, 
               'orientation' : self._toAxisAngleArray(drawOrientation) ,
               'Graphical' : gfxNode}
        self._front = Visual()
        self._front.load((scene, parent, cfg))
        
        # Create Back Pipe
        position = basePos + (frontBackOffset * 1)
        cfg = {'name' : baseName + 'BarbedWireBack', 
               'position' : position, 
               'orientation' : self._toAxisAngleArray(drawOrientation) ,
               'Graphical' : gfxNode}
        self._back = Visual()
        self._back.load((scene, parent, cfg))
        
class Target(ram.sim.object.Object):
    core.implements(ram.sim.object.IObject, ITarget)
    
    # The size of the target opening
    SEPERATION = 0.4572
    
    @two_step_init
    def __init__(self):
        ram.sim.object.Object.__init__(self)
        self._position = ogre.Vector3.ZERO
        self._orientation = ogre.Quaternion.IDENTITY
    
    def _toAxisAngleArray(self, orientation):
        angle = ogre.Degree(0)
        vector = ogre.Vector3()
        orientation.ToAngleAxis(angle, vector)
        return [vector.x, vector.y, vector.z, angle.valueDegrees()]
    
    @property
    def position(self):
        return self._position
    
    @property
    def orientation(self):
        return self._orientation
    
    def load(self, data_object):
        scene, parent, node = data_object
        ram.sim.object.Object.load(self, (parent, node))
        
        # Parse config information
        basepos, orientation = parse_position_orientation(node)
        self._position = ram.sim.OgreVector3(basepos)
        self._orientation = orientation
        basePos = ram.sim.OgreVector3(basepos)
        baseName = node['name']
        
        sideOffset = orientation * ogre.Vector3(0, Target.SEPERATION/2, 0)
        upDownOffset = orientation * ogre.Vector3(0, 0, Target.SEPERATION/2)
        drawOrientation = orientation * ogre.Quaternion(
            ogre.Degree(90), ogre.Vector3.UNIT_Z)
        
        # Shared graphics node
        gfxNode = {'mesh': 'cylinder.mesh', 
                   'material' : 'Simple/Green',
                   'scale': [0.508, 0.0508/2, 0.0508/2] }
        
        # Create Top Pipe
        position = basePos + (upDownOffset * 1)
        cfg = {'name' : baseName + 'TargetTop', 
               'position' : position, 
               'orientation' : self._toAxisAngleArray(drawOrientation) ,
               'Graphical' : gfxNode}
        pipe = Visual()
        pipe.load((scene, parent, cfg))
        
        # Create Bottom Pipe
        position = basePos + (upDownOffset * -1)
        cfg = {'name' : baseName + 'TargetBottom', 
               'position' : position, 
               'orientation' : self._toAxisAngleArray(drawOrientation) ,
               'Graphical' : gfxNode}
        pipe = Visual()
        pipe.load((scene, parent, cfg))

        # Flip them so they point up and down
        drawOrientation = drawOrientation * ogre.Quaternion(
            ogre.Degree(90), ogre.Vector3.UNIT_Y)
        
        # Create Left Pipe
        position = basePos + (sideOffset * 1)
        cfg = {'name' : baseName + 'TargetLeft', 
               'position' : position, 
               'orientation' : self._toAxisAngleArray(drawOrientation) ,
               'Graphical' : gfxNode}
        pipe = Visual()
        pipe.load((scene, parent, cfg))
        
        # Create Right Pipe
        position = basePos + (sideOffset * -1)
        cfg = {'name' : baseName + 'TargetRight', 
               'position' : position, 
               'orientation' : self._toAxisAngleArray(drawOrientation) ,
               'Graphical' : gfxNode}
        pipe = Visual()
        pipe.load((scene, parent, cfg))

class Bin(Visual):
    """
    Represents a single competition bin
    """
    core.implements(IVisual, IBin)
    
    @two_step_init
    def __init__(self):
        Visual.__init__(self)
        self._suit = Suit.NONEFOUND
        self.visible = False
        self.id = 0

    def load(self, data_object):
        scene, parent, node = data_object
        # Automatically fill in the graphical data
        if not node.has_key('Graphical'):
            # Default mesh and scale info
            gfxNode = {'mesh' : 'box.mesh', 'scale' : [0.9144, 0.6096, 0.0254]}
            
            # Determine suit material
            material = 'CompElement/Bin'
            
            _type = node.get('suit', '').lower()
            self._suit = Suit.NONEFOUND
            possibleSuits = set(['heart','spade','club','diamond'])
            if _type in possibleSuits:
                # Added the suit name to the base (first letter caps
                material = 'CompElement/' + _type[0].upper() + _type[1:] + 'Bin'
                
                # Set suit type
                typeToFlag = {'club' : Suit.CLUB, 'heart' : Suit.HEART,
                              'spade' : Suit.SPADE, 'diamond' : Suit.DIAMOND}
                self._suit = typeToFlag[_type]

            gfxNode['material'] = material
                
            # Load the node
            node['Graphical'] = gfxNode
            

        else:
            self._suit = Suit.NONEFOUND
        Visual.load(self, (scene, parent, node))
        
    def save(self, data_object):
        raise "Not yet implemented"
    
    @property
    def suit(self):
        return self._suit
    
class BlackJackTable(ram.sim.object.Object):
    """
    Represents the array of bins in the competition
    """
    core.implements(ram.sim.object.IObject)
    
    SEPERATION = 0.64008
    
    @two_step_init
    def __init__(self):
        ram.sim.object.Object.__init__(self)
        self._farLeftBin = None
        self._leftBin = None
        self._rightBin = None
        self._farRightBin = None

    def _toAxisAngleArray(self, orientation):
        angle = ogre.Degree(0)
        vector = ogre.Vector3()
        orientation.ToAngleAxis(angle, vector)
        return [vector.x, vector.y, vector.z, angle.valueDegrees()]

    def load(self, data_object):
        scene, parent, node = data_object
        ram.sim.object.Object.load(self, (parent, node))
        
        # Parse config information
        basePos, orientation = parse_position_orientation(node)
        basePos = ram.sim.OgreVector3(basePos)
        baseOffset = orientation * ogre.Vector3(0, BlackJackTable.SEPERATION, 0)
        baseName = node['name']
        
        suits = node.get('suits', ['heart','spade','club','diamond'])
        while len(suits) < 4:
            suits.append('')
        
        # Create far left bin
        self._farLeftBin = Bin()
        position = basePos + (baseOffset * -1.5)
        cfg = {'name' : baseName + 'FarLeftBin', 'position' : position, 
               'orientation' : self._toAxisAngleArray(orientation),
               'suit' : suits[0]}
        self._farLeftBin.load((scene, parent, cfg))
        scene._objects.append(self._farLeftBin)
        
        # Create left bin
        self._leftBin = Bin()
        position = basePos + (baseOffset * -0.5)
        cfg = {'name' : baseName + 'LeftBin', 'position' : position, 
               'orientation' : self._toAxisAngleArray(orientation),
               'suit' : suits[1]}
        self._leftBin.load((scene, parent, cfg))
        scene._objects.append(self._leftBin)

        # Create right bin
        self._rightBin = Bin()
        position = basePos + (baseOffset * 0.5)
        cfg = {'name' : baseName + 'RightBin', 'position' : position, 
               'orientation' : self._toAxisAngleArray(orientation),
               'suit' : suits[2]}
        self._rightBin.load((scene, parent, cfg))
        scene._objects.append(self._rightBin)

        # Create right bin
        self._farRightBin = Bin()
        position = basePos + (baseOffset * 1.5)
        cfg = {'name' : baseName + 'FarRightBin', 'position' : position, 
               'orientation' : self._toAxisAngleArray(orientation),
               'suit' : suits[3]}
        self._farRightBin.load((scene, parent, cfg))
        scene._objects.append(self._farRightBin)
        
        
    def save(self, data_object):
        raise "Not yet implemented"
    
class AirDuct(ram.sim.object.Object):
    core.implements(ram.sim.object.IObject, IDuct)
    
    SEPERATION = 0.762
    
    @two_step_init
    def __init__(self):
        ram.sim.object.Object.__init__(self)
    
    def _toAxisAngleArray(self, orientation):
        angle = ogre.Degree(0)
        vector = ogre.Vector3()
        orientation.ToAngleAxis(angle, vector)
        return [vector.x, vector.y, vector.z, angle.valueDegrees()]
    
    @property
    def position(self):
        return self._position
    
    @property
    def orientation(self):
        return self._orientation
    
    def load(self, data_object):
        scene, parent, node = data_object
        ram.sim.object.Object.load(self, (parent, node))
        
        # Parse config information
        basepos, orientation = parse_position_orientation(node)
        self._position = ram.sim.OgreVector3(basepos)
        self._orientation = orientation
        basePos = ram.sim.OgreVector3(basepos)
        baseName = node['name']
        
        sideOffset = orientation * ogre.Vector3(0, AirDuct.SEPERATION, 0)
        upDownOffset = orientation * ogre.Vector3(0, 0, AirDuct.SEPERATION)
        
        # Shared graphics node
        gfxNode = {'mesh': 'box.mesh', 'material' : 'CompElement/AirDuctSide',
                   'scale': [1.524, 1.524, 0.003175] }
        
        # Create Bottom
        position = basePos + (upDownOffset * -1)
        botGfx = gfxNode.copy()
        botGfx['material'] = 'CompElement/AirDuctBottom'
        cfg = {'name' : baseName + 'DuctBottom', 'position' : position, 
               'orientation' : self._toAxisAngleArray(orientation) ,
               'Graphical' : botGfx}
        side = Visual()
        side.load((scene, parent, cfg))
        
        # Create Top
        position = basePos + (upDownOffset * 1)
        cfg = {'name' : baseName + 'DuctTop', 'position' : position, 
               'orientation' : self._toAxisAngleArray(orientation) ,
               'Graphical' : gfxNode}
        side = Visual()
        side.load((scene, parent, cfg))
        
        orientation = orientation * ogre.Quaternion(
            ogre.Degree(90), ogre.Vector3.UNIT_X)
        
        # Create Right
        position = basePos + (sideOffset * -1)
        cfg = {'name' : baseName + 'DuctRight', 'position' : position, 
               'orientation' : self._toAxisAngleArray(orientation) ,
               'Graphical' : gfxNode}
        side = Visual()
        side.load((scene, parent, cfg))
        
        # Create Left
        position = basePos + (sideOffset * 1)
        cfg = {'name' : baseName + 'DuctLeft', 'position' : position, 
               'orientation' : self._toAxisAngleArray(orientation) ,
               'Graphical' : gfxNode}
        side = Visual()
        side.load((scene, parent, cfg))
        
        # Create Left
#        position = basePos + (sideOffset * -1)
#        cfg = {'name' : baseName + 'DuctTop', 'position' : position, 
#               'orientation' : self._toAxisAngleArray(orientation) ,
#               'Graphical' : gfxNode}
#        side = Visual()
#        side.load((scene, parent, cfg))

class Safe(Visual):
    core.implements(IVisual, ISafe)
    
    @two_step_init
    def __init__(self):
        Visual.__init__(self)

    def load(self, data_object):
        scene, parent, node = data_object
        if not node.has_key('Graphical'):
            # Default mesh and scale info
            gfxNode = {'mesh' : 'cube.1m.mesh', 
                       'scale' : [0.3048, 0.3048, 0.3048],
                       'material' : 'Simple/Orange' }
            node['Graphical'] = gfxNode
        Visual.load(self, (scene, parent, node))

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
        self._binID = 1
        
        # Duct Detector variables
        self._runDuct = False
        self._foundDuct = False
        
        # Safe Detector variables
        self._runDownSafeDetector = False
        self._foundDownSafe = False
        #self._pipeCentered = False
        
        # Find all the Buoys, Pipes and Bins
        self._bouys = sim.scene.getObjectsByInterface(IBuoy)
        self._pipes = sim.scene.getObjectsByInterface(IPipe)
        self._bins = sim.scene.getObjectsByInterface(IBin)
        self._ducts = sim.scene.getObjectsByInterface(IDuct)
        self._safes = sim.scene.getObjectsByInterface(ISafe)

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
        
    def ductDetectorOn(self):
        self._runDuct = True
        
    def ductDetectorOff(self):
        self._runDuct = False

    def downwardSafeDetectorOn(self):
        self._runDownSafeDetector = True
        
    def downwardSafeDetectorOff(self):
        self._runDownSafeDetector = False

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
        if self._runDuct:
            self._checkDuct()
        if self._runDownSafeDetector:
            self._checkDownwardSafe()
    
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
        
    def _forwardCheck(self, relativePos, obj):
        """
        Determines the whether or the object at the given relative position
        is is visible, and if so, it returns its x, y (camera frame cordinates)
        and angle.
        
        @type  relativePos: ogre.renderer.OGRE.Vector3
        @param relativePos: The relative position to the downward object
        
        @rtype:  (bool, double, double, ext.math.Degree)
        @return: (visible, x, y, angle)
        """
        
        forwardVector = self.vehicle.robot.orientation * ogre.Vector3.UNIT_X
        
        quat = forwardVector.getRotationTo(relativePos)
        yaw = -quat.getRoll(True).valueDegrees()
        pitch = quat.getYaw(True).valueDegrees()
        
        # Check to see if its the field of view
        visible = False
        x = 0.0
        y = 0.0
        angle = ext.math.Degree(0.0)
        
        # Check to see if its the field of view
        if (math.fabs(yaw) <= (self._horizontalFOV/2)) and \
           (math.fabs(pitch) <= (self._verticalFOV/2)):
            visible = True
        
            x = yaw / (self._horizontalFOV/2)
            # Negative because of the corindate system
            y = -pitch / (self._verticalFOV/2)
        
            # Find relative angle
            forwardObj = obj.orientation * ogre.Vector3.UNIT_X
            orientation = forwardVector.getRotationTo(forwardObj)
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
        found = False
        visibleBins = 0
        lastAngle = ext.math.Degree(0)
        
        for bin in self._bins:
            relativePos = bin.position - self.vehicle.robot.position
            binVisible, x, y, angle = self._downwardCheck(relativePos, bin)

            if binVisible and (relativePos.length() < 4.5):
                visibleBins += 1
                found = True

                id = self._binID
                self._binID += 1
                if not bin.visible:
                    bin.id = id
                    bin.visible = True
                else:
                    id = bin.id
                
                # Publish found event
                event = ext.core.Event()
                event.x = x
                event.y = y
                event.angle = angle
                event.suit = bin.suit
                event.id = id
                self.publish(ext.vision.EventType.BIN_FOUND, event)
                
                # Record angle for use in the multi bin event
                lastAngle = angle
            elif bin.visible:
                event = ext.core.Event()
                event.id = bin.id
                self.publish(ext.vision.EventType.BIN_DROPPED, event)
                bin.visible = False
                
        # Multi Bin Angle Event
        if visibleBins > 1:
            event = ext.core.Event()
            event.angle = lastAngle
            self.publish(ext.vision.EventType.MULTI_BIN_ANGLE, event)
        
        # Lost Event
        if self._foundBin and (not found):
            self.publish(ext.vision.EventType.BIN_LOST, ext.core.Event())

        # Check for centering
        
        # Sort list to find the closest bin
        def cmp(a,b):
            relativePosA = a.position - self.vehicle.robot.position
            relativePosB = b.position - self.vehicle.robot.position
            lenA = relativePosA.length()
            lenB = relativePosB.length()
            if lenA < lenB:
                return -1
            elif lenA > lenB:
                return 1
            else:
                return 0
        sort = sorted(self._bins, cmp)
        
        # Transform to image space
        closestBin = sort[0]
        relativePos = closestBin.position - self.vehicle.robot.position
        binVisible, x, y, angle = self._downwardCheck(relativePos, closestBin)

        toCenter = ogre.Vector2(x,y)
        if binVisible and (toCenter.normalise() < 0.2):
            if not self._binCentered:
                self._binCentered = True
                event = ext.core.Event()
                event.id = closestBin.id
                self.publish(ext.vision.EventType.BIN_CENTERED, event)
            else:
                self._binCentered = False

        self._foundBin = found
        
    def _checkDuct(self):
        duct, relativePos = self._findClosest(self._ducts)
        ductVisible, x, y, angle = self._forwardCheck(relativePos, duct)

        if ductVisible and (relativePos.length() < 8):
            event = ext.vision.DuctEvent(0.0, 0.0, 0.0, 0.0, False, False)
            event.x = x
            event.y = y
            
            # Runs 1 -> 0, getting bigger the closer you go
            event.range = 1 - ((8 - relativePos.length()) / 8.0)
            
            # TODO: Scale me
            angleDeg = angle.valueDegrees() * -1
            if angleDeg > 90:
                angleDeg -= 180
            elif angleDeg < -90:
                angleDeg += 180
            
            event.alignment = angleDeg
            
            # Determine if we are aligned (with perpendicular offset)
            forwardVector = duct.orientation * ogre.Vector3.UNIT_X
            offset = relativePos.crossProduct(forwardVector).length()
            
            if (offset < 2) and (math.fabs(angleDeg) < 3):
                event.aligned = True
            else:
                event.aligned = False
            event.visible = True
            
            self.publish(ext.vision.EventType.DUCT_FOUND, event)
            
#            # Check for centering
#            toCenter = ogre.Vector2(x, y)
#            if toCenter.normalise() < 0.08:
#                if not self._ductCentered:
#                    self._ductCentered = True
#                    self.publish(ext.vision.EventType.duct_CENTERED, event)
#            else:
#                self._ductCentered = False
            
        else:
            if self._foundDuct:
                self.publish(ext.vision.EventType.DUCT_LOST, ext.core.Event())

        self._foundDuct = ductVisible
        
    def _checkDownwardSafe(self):
        safe, relativePos = self._findClosest(self._safes)
        safeVisible, x, y, angle = self._downwardCheck(relativePos, safe)

        if safeVisible and (relativePos.length() < 7):
            event = ext.core.Event()
            event.x = x
            event.y = y
            #event.angle = angle
            self.publish(ext.vision.EventType.SAFE_FOUND, event)
            
            # Check for centering
#            toCenter = ogre.Vector2(x, y)
#            if toCenter.normalise() < 0.08:
#                if not self._safeCentered:
#                    self._safeCentered = True
#                    self.publish(ext.vision.EventType.PIPE_CENTERED, event)
#            else:
#                self._safeCentered = False
            
        else:
            if self._foundDownSafe:
                self.publish(ext.vision.EventType.SAFE_LOST, ext.core.Event())

        self._foundDownSafe = safeVisible
        

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
        viewport = rttTex.addViewport(camera)
        viewport.setBackgroundColour(ogre.ColourValue(200,200,200))

        setattr(self, camera.name + 'Texture', texture)

    def _createCamera(self, name, position, direction):
        """Lets hack on a camera (integrate better in the future)"""
        
        node = self.vehicle.robot._main_part._node
        sceneMgr = ogre.Root.getSingleton().getSceneManager('Main')

        # Create camera and attached it to our ourself
        camera = sceneMgr.createCamera(name)

        # Align and Position
        camera.position = position
        camera.lookAt(camera.position + ram.sim.OgreVector3(direction))
        camera.nearClipDistance = 0.05
        node.attachObject(camera)

        # This needs be set from the config file (only VERTICAL FOV)
        camera.FOVy = ogre.Degree(78)

        # NOTE: Fix not needed because camera on the vehicle is offset in the
        #       same way, what an odd coincidence
        # Account for the odd up vector difference between our and Ogre's 
        # default coordinate systems
        camera.roll(ogre.Degree(-90))

        return camera

    def saveForwardCameraSnapshot(self, filename):
        """
        Saves the current forward camera image to file
        """
        address = ctypes.addressof(self._forwardBuffer)
        image = ext.vision.Image.loadFromBuffer(address, 640, 480, False)
        ext.vision.Image.saveToFile(image, filename)


ext.core.SubsystemMaker.registerSubsystem('SimVision', SimVision)
