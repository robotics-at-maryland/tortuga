# 
# SimpleScenes_BoxStack.h
# ----------------------
# A reimplementation of the ODE box stacking (actually loads of different
# objects) demo using Ogre and the OgreOde wrapper.
#
# 
# The box stacking test extends the base test class
# 
import SimpleScenes.py

class SimpleScenes_BoxStack ( SimpleScenes ):
    def __init__ ( self, world ):
        SimpeScenes.__init__( self, world )
    def __del__ ( self ):
        pass
        
    ## Return our name for the test application to display
    def getName(self ):
        return "Test Box Stack"

    ## Return a description of the keys that the user can use in this test
    def getKeys( self ):
        return "Z - Sphere, X - Box, C - Capsule, G - Ragdoll, T - Trimesh"

    def frameEnded(self, time, keyinput,  mouse)
    {
        ## Do default processing
        SimpleScenes::frameEnded(self,time, keyinput, mouse)
        body = None
    
        ## Create random objects, that method will stop them being created too often
        if (keyinput.isKeyDown(OIS.KC_Z)) 
            body = createRandomObject(OgreOde::Geometry::Class_Sphere)
        else if (keyinput.isKeyDown(OIS.KC_X)) 
            body = createRandomObject(OgreOde::Geometry::Class_Box)
        else if (keyinput.isKeyDown(OIS.KC_C)) 
            body = createRandomObject(OgreOde::Geometry::Class_Capsule)
        else if (keyinput.isKeyDown(OIS.KC_T)) 
            body = createRandomObject(OgreOde::Geometry::Class_TriangleMesh)
        else if (keyinput.isKeyDown(OIs.KC_G)) 
            createRagDoll()
        
