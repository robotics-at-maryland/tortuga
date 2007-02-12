# 
# SimpleScenes_BoxStack.h
# ----------------------
# A reimplementation of the ODE box stacking (actually loads of different
# objects) demo using Ogre and the OgreOde wrapper.
#
# 
# The box stacking test extends the base test class
# 
from SimpleScenes import *##.py
import Ogre as ogre
import OgreOde
import OIS


class SimpleScenes_BoxStack ( SimpleScenes ):
    def __init__ ( self, world ):
        SimpleScenes.__init__( self, world )
    def __del__ ( self ):
        SimpleScenes.__del__(self)
       
    ## Return our name for the test application to display
    def getName(self ):
        return "Test Box Stack"

    ## Return a description of the keys that the user can use in this test
    def getKeys( self ):
        return "Z - Sphere, X - Box, C - Capsule, G - Ragdoll, T - Trimesh"

    def frameEnded(self, time, keyinput,  mouse):
        ## Do default processing
        SimpleScenes.frameEnded(self, time, keyinput, mouse)
        body = None
        
        ## Create random objects, that method will stop them being created too often
        if (keyinput.isKeyDown(OIS.KC_Z)): 
            body = self.createRandomObject(OgreOde.Geometry.Class_Sphere)
        elif (keyinput.isKeyDown(OIS.KC_X)): 
            body = self.createRandomObject(OgreOde.Geometry.Class_Box)
        elif (keyinput.isKeyDown(OIS.KC_C)): 
            body = self.createRandomObject(OgreOde.Geometry.Class_Cylinder) #Capsule
        elif (keyinput.isKeyDown(OIS.KC_T)): 
            body = self.createRandomObject(OgreOde.Geometry.Class_Convex) #TriangleMesh)
        elif (keyinput.isKeyDown(OIS.KC_G)): 
            self.createRagDoll()
        
        