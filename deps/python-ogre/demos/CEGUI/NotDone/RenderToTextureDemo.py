# this code is in the public domain

#------------------------------------------------------------------------------
#  This sample shows creation of Render To Texture:
#
#     For windows users:
#           - creates RTT frame for 3 schemes "WindowsLook","TaharezLook","VanillaSkin"
#     For Linux users:
#           - creates RTT 3 "TaharezLook" frames until library loading problem resolved
#
#-----------------------------------------------------------------------------

from pyogre import ogre,cegui
import SampleFramework
import os


def _PointHack(x, y):
    return cegui.Vector2(x, y)
cegui.Point = _PointHack


 
class CEGUIFrameListener(SampleFramework.FrameListener):

    def __init__(self, renderWindow, camera, sceneManager):

        SampleFramework.FrameListener.__init__(self, renderWindow, camera)
        self.keepRendering = True   # whether to continue rendering or not
        self.sceneManager = sceneManager

    def _setupInput(self):
        self.eventProcessor = ogre.EventProcessor()
        self.eventProcessor.initialise(self.renderWindow)
        self.eventProcessor.startProcessingEvents()

        # register as a listener for events
        self.eventProcessor.addKeyListener(self)
        self.eventProcessor.addMouseListener(self)
        self.eventProcessor.addMouseMotionListener(self)


    def frameStarted(self, evt):
        return self.keepRendering

    def mouseDragged(self, evt):
        self.mouseMoved(evt)

    def mousePressed(self, evt):
        button = self._convertOgreButtonToCegui(evt)
        cegui.System.getSingleton().injectMouseButtonDown(button)

    def mouseReleased(self, evt):
        button = self._convertOgreButtonToCegui(evt)
        cegui.System.getSingleton().injectMouseButtonUp(button)
	
    def mouseClicked(self, evt):
        pass

    def mouseEntered(self, evt):
        pass

    def mouseExited(self, evt):
        pass

    def mouseMoved(self, evt):
        system = cegui.System.getSingleton()
        renderer = system.renderer
        system.injectMouseMove(evt.relX * renderer.width,
                               evt.relY * renderer.height)


    def keyPressed(self, evt):
        if evt.key == ogre.KC_ESCAPE:
            self.keepRendering = False
        system = cegui.System.getSingleton()
        system.injectKeyDown(evt.key)
        system.injectChar(evt.keyChar)
        evt.consume()

    def keyReleased(self, evt):
        system = cegui.System.getSingleton()
        system.injectKeyUp(evt.key)

    def keyClicked(self, evt):
        pass

    def _convertOgreButtonToCegui(self,evt):
	# Convert ogre button to cegui button
        if (evt.buttonID & ogre.MouseEvent.BUTTON0_MASK):
            return cegui.LeftButton	
            print "Left Button"
        elif (evt.buttonID & ogre.MouseEvent.BUTTON1_MASK):
            return cegui.RightButton	
        elif (evt.buttonID & ogre.MouseEvent.BUTTON2_MASK):
            return cegui.MiddleButton
        elif (evt.buttonID & ogre.MouseEvent.BUTTON3_MASK):
            return cegui.X1Button
        return cegui.LeftButton

   
class GEUIApplication(SampleFramework.Application):

    def __init__(self):
        SampleFramework.Application.__init__(self)
        self.guiRenderer=0
        self.system =0


		
		
    def _initGUI(self):
        # Initiaslise CEGUI Renderer
        self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow,ogre.RENDER_QUEUE_OVERLAY, False, self.sceneManager)		
        self.system = cegui.System(self.guiRenderer)
        cegui.Logger.getSingleton().loggingLevel = cegui.Insane
	
        # create font
        cegui.FontManager.getSingleton().createFont("Iconified-12.font")
        # Load Default CEGUI Window
        sheet = cegui.WindowManager.getSingleton().createWindow("DefaultWindow", "root_wnd")
        self.system.guiSheet = sheet

    def _createRTT(self):

        # Setup Render To Texture (RTT) for preview window
        rttTex = self.root.renderSystem.createRenderTexture("RttTex", 512, 512) # TEX_TYPE_2D, PF_R8G8B8 )
        rttCam = self.sceneManager.createCamera("RttCam")

        # create camera node for RTT
        camNode = self.sceneManager.rootSceneNode.createChildSceneNode("rttCamNode")
        camNode.attachObject(rttCam)
        rttCam.position =0,0,200

        # create RTT viewport
        v = rttTex.addViewport( rttCam )
        v.overlaysEnabled = False
        v.clearEveryFrame = True
        v.backgroundColour =ogre.ColourValue.Black

        # get CEGUI renderer
        renderer = cegui.System.getSingleton().renderer

        # create CEGUI texture and imageset
        rttTexture = renderer.createTexture("RttTex","General")
        rttImageSet = cegui.ImagesetManager.getSingleton().createImageset("RttImageset", rttTexture)
        rttImageSet.defineImage("RttImage", cegui.Point(0.0, 0.0), cegui.Size(rttTex.width, rttTex.height), \
                   cegui.Point(0.0,0.0))

    def _createRTTGUIObject(self,control_count,look):

        # look          :  WindowsLook, "TaharezLook", "Vanilla"
        # control_count :  counter to ensure unique widget names   
        # eg            :  createWindow(look + "/StaticImage", 'RTTImage_%d' % control_count)
        # equivalent to    createWindow("WindowsLook/StaticImage", 'RTTImage_1')

        # get main gui window and RTT imageset
        sheet = self.system.guiSheet
        rttImageSet=cegui.ImagesetManager.getSingleton().getImageset("RttImageset")

        # create static image & frame           
        si=cegui.WindowManager.getSingleton().createWindow(look + "/StaticImage", 'RTTImage_%d' % control_count)
        frame=cegui.WindowManager.getSingleton().createWindow(look+"/FrameWindow", 'Frame_%d' % control_count)

        # set static image characteristics
        si.position=cegui.Point(0.1,0.15)
        si.size=cegui.Size(0.8, 0.8)
        si.image=(rttImageSet.getImage("RttImage"))
        si.visible =True

        # set frame characteristics
        frame.frameEnabled=True
        frame.size=cegui.Size(0.4, 0.4)
        frame.position=cegui.Point(0.2 + (control_count-1)*0.2, 0.2+(control_count-1)*0.2)
        frame.visible =True
        frame.alpha=0.5
        frame.text = "Example Render to Texture"

        # add frame to main window
        sheet.addChildWindow(frame)

        # add static image to frame
        frame.addChildWindow(si)

    def _createScene(self):
        sceneManager = self.sceneManager
        sceneManager.ambientLight = 0.8, 0.8, 0.8
        #sceneManager.setSkyDome(True, 'Examples/CloudySky', 4.0, 8.0)

        light = sceneManager.createLight('MainLight')
        light.position = 20, 80, 50
        self.viewport.backgroundColour = (0.5,0.5,0.5)

        # create head entity
        headNode = sceneManager.rootSceneNode.createChildSceneNode("headNode")
        entity = sceneManager.createEntity('head', 'ogrehead.mesh')
        headNode.attachObject(entity)
        headNode.translate(ogre.Vector3(0,0,-100))

        self._initGUI()       # initialise GUI renderer
        self._createRTT()     # create Render to texture

        # load 3 different schemes
        cegui.SchemeManager.getSingleton().loadScheme("TaharezLookSkin.scheme")
        cegui.SchemeManager.getSingleton().loadScheme("WindowsLookSkin.scheme")
        cegui.SchemeManager.getSingleton().loadScheme("VanillaSkin.scheme")
        # use TaharezLook Mouse Cursor
        self.system.defaultMouseCursor = ("TaharezLook", "MouseArrow")

	print 5
        # Create RTTGUI object for each scheme
        self._createRTTGUIObject(0,"TaharezLook")
        self._createRTTGUIObject(1,"WindowsLook")
        self._createRTTGUIObject(2,"Vanilla")



    def _createCamera(self):
        self.camera = self.sceneManager.createCamera("PlayerCam")
        self.camera.nearClipDistance = 5

    def _createFrameListener(self):
        self.frameListener = CEGUIFrameListener(self.renderWindow, self.camera, self.sceneManager)
        self.root.addFrameListener(self.frameListener)
        self.frameListener.showDebugOverlay(True)

    def __del__(self):
        "Clear variables, this is needed to ensure the correct order of deletion"
        del self.camera
        del self.sceneManager
        del self.frameListener
        del self.system
        del self.guiRenderer
        del self.root
        del self.renderWindow        
     

if __name__ == '__main__':
    try:
        ta = GEUIApplication()
        ta.go()
    except ogre.OgreException, e:
        print e
