# /*
#     OgreNewt library - connecting Ogre and Newton!

#     Demo04_Raycasting - how raycasting in Newton works, implemented with a simple way to drag objects around.
# */
import Ogre
import OgreNewt
import OIS
import CEGUI
import random
import SampleFramework as sf
from BasicFrameListener import *     # a simple frame listener that updates physics as required..
from CEGUI_framework import *   ## we need the OIS version of the framelistener etc

holder=[]

class OgreNewtonApplication (sf.Application):
    def __init__ ( self):
        sf.Application.__init__(self)
        self.World = OgreNewt.World()
        self.EntityCount = 0
        self.bodies=[]
        self.frameListener = None
        self.msnCam =None
        

    def __del__(self):
        ##
        ## important that things get deleted in the right order
        ##
        if self.camera:
            del self.camera
        del self.sceneManager
        del self.frameListener
        try:
            if self.EditorGuiSheet:
                CEGUI.WindowManager.getSingleton().destroyWindow(self.EditorGuiSheet) 
        except:
            pass
        del self.GUIsystem
        del self.GUIRenderer
        del self.root
        del self.renderWindow        
        ## delete the world when we're done.
        del self.bodies
        del self.World
    
        ## de-initialize the debugger.
    #   OgreNewt.Debugger.getSingleton().deInit()
        sf.Application.__del__(self)


    def _createScene ( self ):
    
        
        ## setup CEGUI
        self.GUIRenderer = CEGUI.OgreCEGUIRenderer( self.renderWindow, 
                Ogre.RENDER_QUEUE_OVERLAY, False, 3000, self.sceneManager )
        self.GUIsystem = CEGUI.System( self.GUIRenderer )
        ## load up CEGUI stuff...
        CEGUI.Logger.getSingleton().setLoggingLevel( CEGUI.Informative )
#         CEGUI.SchemeManager.getSingleton().loadScheme("WindowsLook.scheme") #../../Media/GUI/schemes/WindowsLook.scheme")
#         self.GUIsystem.setDefaultMouseCursor("WindowsLook", "MouseArrow")
#         self.GUIsystem.setDefaultFont("Commonwealth-10")
        CEGUI.SchemeManager.getSingleton().loadScheme("TaharezLookSkin.scheme") 
        self.GUIsystem.setDefaultMouseCursor("TaharezLook",  "MouseArrow") 
        self.GUIsystem.setDefaultFont( "BlueHighway-12") 

            
        sheet = CEGUI.WindowManager.getSingleton().createWindow( "DefaultWindow", "root_wnd" )
        CEGUI.System.getSingleton().setGUISheet( sheet )
    

        
        ## sky box.
        self.sceneManager.setSkyBox(True, "Examples/CloudyNoonSkyBox")
        
        ## shadows on!
        self.sceneManager.setShadowTechnique( Ogre.SHADOWTYPE_STENCIL_ADDITIVE )
        
        ## floor object!
        floor = self.sceneManager.createEntity("Floor", "simple_terrain.mesh" )
        floornode = self.sceneManager.getRootSceneNode().createChildSceneNode( "FloorNode" )
        floornode.attachObject( floor )
        floor.setMaterialName( "Simple/BeachStones" )
        floor.setCastShadows( False )
    
        ##Ogre.Vector3 siz(100.0, 10.0, 100.0)
        print self.World, floornode
        
        col = OgreNewt.TreeCollision( self.World, floornode, True )
        
        bod = OgreNewt.Body( self.World, col )
        del col
        
        ##floornode.setScale( siz )
        bod.attachToNode( floornode )
        bod.setPositionOrientation( Ogre.Vector3(0.0,-10.0,0.0), Ogre.Quaternion.IDENTITY )
        self.bodies.append(bod)
        
        ## make a simple rope.
        size=Ogre.Vector3 (3,1.0,1.0)
        pos=Ogre.Vector3 (0,1,0)
        orient = Ogre.Quaternion.IDENTITY
    
        ## loop through, making bodies and connecting them.
        parent = None
        child = None
        
        
        ## we need to make the framelistener early to get access to the call backs..
#         self.frameListener = OgreNewtonFrameListener( self.renderWindow, self.camera, 
#                             self.sceneManager, self.World, self.msnCam, self.GUIRenderer )
    
        for x in range(8):
            ## make the next box.
            child = self.makeSimpleBox(size, pos, orient)
            self.bodies.append(child)
            ## set the buoyancy callback
            ##child.setCustomForceAndTorqueCallback( standardForceCallback, "")
            child.setCustomForceAndTorqueCallback( standardForceCallback, "")
               
            ## make the joint right between the bodies...
            if parent :
                joint = OgreNewt.BallAndSocket( self.World, child, parent, pos-Ogre.Vector3(size.x/2,0,0) )
            else:
                ## no parent, this is the first joint, so just pass None as the parent, to stick it to the "world"
                joint = OgreNewt.BallAndSocket( self.World, child, None, pos-Ogre.Vector3(size.x/2,0,0) )
                
            ## need to save the joint, ohterwise it won't work...
            self.bodies.append(joint)
            ## offset pos a little more.
            pos += Ogre.Vector3(size.x,0,0)
            ## save the last body for the next loop!
            parent = child

#         OgreNewt.TestOne(self.root, self.renderWindow, self.sceneManager, self.World ) 
    
        for i in range(15):
            pos = Ogre.Vector3( 10-random.random()%20, 4+random.random()%2, 10-random.random()%20 )
            size = Ogre.Vector3( 1+random.random()%3, 1+random.random()%3, 1+random.random()%3 )
    
            bod = self.makeSimpleBox( size, pos, orient )

            ## set the buoyancy callback
            bod.setCustomForceAndTorqueCallback(  standardForceCallback,  "")
            self.bodies.append ( bod)
            
        ## make a simple visual object for the buoyancy plane.
        theplane = self.sceneManager.createEntity("ThePlane", Ogre.SceneManager.PT_PLANE )
        planeNode = self.sceneManager.getRootSceneNode().createChildSceneNode()
    
        theplane.setMaterialName( "Simple/Translucent" )
        planeNode.attachObject( theplane )
        planeNode.setPosition( Ogre.Vector3(0,0,0) )
        planeNode.setOrientation( Ogre.Quaternion( Ogre.Degree(d=-90), Ogre.Vector3(1,0,0) ) )
        
        ## position camera
        self.msnCam = self.sceneManager.getRootSceneNode().createChildSceneNode()
        self.msnCam.attachObject( self.camera )
        self.camera.setPosition(0.0, 5.0, 10.0)
        #self.camera.lookAt (0,0,0)
        self.msnCam.setPosition( 0.0, -3.0, 23.0)
    
        ##self.frameListener.msnCam = self.msnCam
        
        ##make a light
        light = self.sceneManager.createLight( "Light1" )
        light.setType( Ogre.Light.LT_POINT )
        light.setPosition( Ogre.Vector3(0.0, 100.0, 100.0) )
            
    

    def makeSimpleBox( self, size, pos,  orient ):
        global holder
        ## base mass on the size of the object.
        mass = size.x * size.y * size.z * 0.5
            
        ## calculate the inertia based on box formula and mass
        inertia = OgreNewt.CalcBoxSolid( mass, size )
     	           
        box1 = self.sceneManager.createEntity( "Entity"+str(self.EntityCount), "box.mesh" )
        self.EntityCount += 1
        box1node = self.sceneManager.getRootSceneNode().createChildSceneNode()
        box1node.attachObject( box1 )
        box1node.setScale( size )
        box1.setNormaliseNormals(True)
        
        col = OgreNewt.Box( self.World, size )
        bod = OgreNewt.Body( self.World, col )
        #del col
                    
        bod.attachToNode( box1node )
        
        bod.setMassMatrix( mass, inertia )
        bod.setStandardForceCallback()
        
        box1.setMaterialName( "Simple/BumpyMetal" )
    
        bod.setPositionOrientation( pos, orient )
    
        return bod

    def _createFrameListener(self):
        
        self.frameListener = OgreNewtonFrameListener( self.renderWindow, self.camera, 
                            self.sceneManager, self.World, self.msnCam, self.GUIRenderer )
        self.root.addFrameListener(self.frameListener)
	
        self.NewtonListener = BasicFrameListener( self.renderWindow, self.sceneManager, self.World, 60 )
        self.root.addFrameListener(self.NewtonListener)

    def destroyScene():
        sys = CEGUI.System.getSingletonPtr()
        del sys
    
        ## CEGUI Cleanup
        del self.GUIRenderer



class OgreNewtonFrameListener(GuiFrameListener ):
    def __init__(self, renderWindow, camera, Mgr, World, msnCam, guirenderer):

        GuiFrameListener.__init__(self, renderWindow, camera, guirenderer)
        self.World = World
        self.msnCam = msnCam
        self.camera= camera
        self.sceneManager = Mgr
        self.timer=0
        self.count=0
        self.bodies=[]

        self.dragging = False
    
        ## setup 3D line node.
        self.DragLineNode = self.sceneManager.getRootSceneNode().createChildSceneNode()
        self.DragLine = Ogre.ManualObject( "__DRAG_LINES__" )
        
        self.dragPoint = None
        self.dragDist = None
        self.dragBody =None


    def frameStarted(self, frameEvent):
        #sf.FrameListener.frameStarted(self, frameEvent)

        ##Need to capture/update each device - this will also trigger any listeners
        ## OIS specific !!!!
        self.Keyboard.capture()    
        self.Mouse.capture()
        ms = self.Mouse.getMouseState()

        ## ----------------------------------------
        ## CAMERA CONTROLS
        ## ----------------------------------------
        quat = self.msnCam.getOrientation()
        vec = Ogre.Vector3(0.0,0.0,-0.5)
        trans = quat * vec
        vec = Ogre.Vector3(0.5,0.0,0.0)
        strafe = quat * vec
        if ((self.Keyboard.isKeyDown(OIS.KC_LSHIFT)) or (self.Keyboard.isKeyDown(OIS.KC_RSHIFT))):
            
            ## now lets handle mouse input
            self.msnCam.pitch( Ogre.Radian(ms.Y.rel * -0.5) )
            self.msnCam.yaw( Ogre.Radian(ms.X.rel * -0.5), Ogre.Node.TS_WORLD )

            ##and Keyboard
        if (self.Keyboard.isKeyDown(OIS.KC_UP)):
            self.msnCam.translate(trans)
        if (self.Keyboard.isKeyDown(OIS.KC_DOWN)):
            self.msnCam.translate(trans * -1.0)
        if (self.Keyboard.isKeyDown(OIS.KC_LEFT)):
            self.msnCam.translate(strafe * -1.0)
        if (self.Keyboard.isKeyDown(OIS.KC_RIGHT)):
            self.msnCam.translate(strafe)


        ## ----------------------------------------
        ## DRAGGING!
        ## ----------------------------------------
    
        if ( not self.dragging):
 
            ##user pressing the left mouse button?
            if (ms.buttonDown(OIS.MouseButtonID.MB_Left)):
                ## perform a raycast!
                ## start at the camera, and go for 100 units in the Z direction.
     
                mouse = CEGUI.MouseCursor.getSingleton().getPosition()
                rend = CEGUI.System.getSingleton().getRenderer()
                
                mx = mouse.d_x / rend.getWidth()
                my = mouse.d_y / rend.getHeight()
                camray = self.camera.getCameraToViewportRay(mx,my)
    
                start = camray.getOrigin()
                end = camray.getPoint( 100.0 )
    
                ray = OgreNewt.BasicRaycast( self.World, start, end )
                info = ray.getFirstHit()
                if (info.mBody):
                    ## a body was found.  first let's find the point we clicked, in local coordinates of the body.
                    ## while self.dragging, make sure the body can't fall asleep.
                    info.mBody.unFreeze()
                    info.mBody.setAutoFreeze(0)
                    bodpos, bodorient = info.mBody.getPositionOrientation( )
                    ## info.mDistance is in the range [0,1].
                    globalpt = camray.getPoint( 100.0 * info.mDistance )
                    localpt = bodorient.Inverse() * (globalpt - bodpos)
    
                    ## now we need to save this point to apply the spring force, I'm using the userData of the bodies in this example.
                    info.mBody.setUserData( self )
                    ## now change the force callback from the standard one to the one that applies the spring (drag) force.
                    info.mBody.setCustomForceAndTorqueCallback( self, "dragCallback" )
    
                    ## save the relevant drag information.
                    self.dragBody = info.mBody
                    self.dragDist = (100.0 * info.mDistance)
                    self.dragPoint = localpt
    
                    self.dragging = True
                del ray
            if (self.DragLine):
                self.remove3DLine()
        else:
            ## currently self.dragging!
            if (not ms.buttonDown(OIS.MouseButtonID.MB_Left)):
                ## no longer holding mouse button, so stop self.dragging!
                ## remove the special callback, and put it back to standard gravity.
                self.dragBody.setCustomForceAndTorqueCallback(  standardForceCallback,  "")
                ##self.dragBody.setStandardForceCallback()
                self.dragBody.setAutoFreeze(1)
    
                self.dragBody = None
                self.dragPoint = Ogre.Vector3.ZERO
                self.dragDist = 0.0
    
                self.dragging = False
    
        if (self.Keyboard.isKeyDown(OIS.KC_ESCAPE)):
            return False
        return True

    def remove3DLine(self):
        self.DragLineNode.detachAllObjects()
        self.DragLine.clear()

    def dragCallback(  self, body ):
        ## this body is being dragged by the mouse, so apply a spring force.
        fl = body.getUserData()
        ## first find the global point the mouse is at...
        mouse = CEGUI.MouseCursor.getSingleton().getPosition()
        rend = CEGUI.System.getSingleton().getRenderer()
        
        mx = mouse.d_x / rend.getWidth()
        my = mouse.d_y / rend.getHeight()
        camray = self.camera.getCameraToViewportRay( mx, my )
    
        campt = camray.getPoint( fl.dragDist )
    
        ## now find the global point on the body:
        bodpos, bodorient = body.getPositionOrientation(  )
        bodpt = (bodorient * fl.dragPoint) + bodpos
            
        ## apply the spring force!
        mass, inertia= body.getMassMatrix(  )
    
        dragForce =  ((campt - bodpt) * mass * 8.0) - body.getVelocity()
    
        ## draw a 3D line between these points for visual effect :)
        self.remove3DLine()
        self.DragLine.begin("BaseWhiteNoLighting", Ogre.RenderOperation.OT_LINE_LIST )
        self.DragLine.position( campt )
        self.DragLine.position( bodpt )
        self.DragLine.end()
        self.DragLineNode.attachObject( self.DragLine )
    
        ## Add the force!
        body.addGlobalForce( dragForce, bodpt )
    
        gravity = Ogre.Vector3(0,-9.8,0) * mass
        body.addForce( gravity )
        ## also don't forget buoyancy force. 
        body.addBouyancyForce( 0.7, 0.5, 0.5, Ogre.Vector3(0.0,-9.8,0.0), 
                     buoyancyCallback, "" )

def standardForceCallback(  me ):
    mass, inertia = me.getMassMatrix( )

    gravity = Ogre.Vector3(0,-9.8,0) * mass
    me.addForce( gravity )

    ## also don't forget buoyancy force.
    ## just pass the acceleration due to gravity, not the force (accel * mass)! 
    me.addBouyancyForce( 0.7, 0.5, 0.5, Ogre.Vector3(0.0,-9.8,0.0), 
                    buoyancyCallback, "" )


    
##################################################################################
##      BUOYANCY CALLBACK
################################################################################/
def buoyancyCallback(  colID,  me,  orient,  pos,  plane ):
    ## here we need to create an Ogre::Plane object representing the surface of the liquid.  in our case, we're 
    ## just assuming a completely flat plane of liquid, however you could use this function to retrieve the plane
    ## equation for an animated sea, etc.
    ##
    plane1 = Ogre.Plane( Ogre.Vector3(0,1,0), Ogre.Vector3(0,0,0) )
    ## we need to copy the normals and 'd' to the plane we were passed...
    plane.normal = plane1.normal
    plane.d = plane1.d
   
    return True
    
        
        
if __name__ == '__main__':
    import exceptions
    try:
        application = OgreNewtonApplication()
        application.go()
    except Ogre.Exception, e:
        print e
        print dir(e)
    except exceptions.RuntimeError, e:
        print "Runtime error:", e
    except exceptions.TypeError, e:
        print "Type error:", e
    except exceptions.AttributeError, e:
        print "Attribute error:", e
    except exceptions.NameError, e:
        print "Name error:", e
    except Exception,inst:
        print "EException"
        print type(inst)     # the exception instance
        print inst.args      # arguments stored in .args
        print inst
    except exceptions.ValueError,e:
        print "ValueError",e
    except :
        print "Unexpected error:", sys.exc_info()[0]
   
                