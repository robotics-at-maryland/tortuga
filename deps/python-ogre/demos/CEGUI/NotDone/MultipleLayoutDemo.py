# this code is in the public domain

#------------------------------------------------------------------------------
#  This sample shows how to break up layouts and reuse. 
#  The demo loads:
#      1)  Default.layout     - the default CEGUI window 
#          InfoBox.layout     - three instances of this layout which contain a 
#                               Frame/StaticText/OK/Cancel Button 
#      2)  Deletes one of the infoboxes and disconnects connection
#      3)  Checks the deadpool 
#
#-----------------------------------------------------------------------------

import Ogre as ogre
import CEGUI as cegui
import SampleFramework


def _PointHack(x, y):
    return cegui.Vector2(x, y)
cegui.Point = _PointHack


# Event Handling
def onButton(args):
    print '---Quit---'
    print `args`

class MouseListener ( OIS.MouseListener ):
    def __init__(self):
        OIS.MouseListener.__init__( self)

    ##----------------------------------------------------------------##
    def mouseMoved( self, arg ):
        CEGUI.System.getSingleton().injectMouseMove( arg.get_state().relX, arg.get_state().relY )
        return True

    ##----------------------------------------------------------------##
    def mousePressed(  self, arg, id ):
        CEGUI.System.getSingleton().injectMouseButtonDown(convertOISMouseButtonToCegui(id))
        return True

    ##----------------------------------------------------------------##
    def mouseReleased( self, arg, id ):
        CEGUI.System.getSingleton().injectMouseButtonUp(convertOISMouseButtonToCegui(id))
        return True
        
            
class CEGUIFrameListener(SampleFramework.FrameListener,OIS.KeyListener, OIS.MouseListener):

    def __init__(self, renderWindow, camera, sceneManager):
#        SampleFramework.FrameListener.__init__(self, renderWindow, camera)
        SampleFramework.FrameListener.__init__(self, renderWindow, camera, True, True, True)
        OIS.KeyListener.__init__(self)
        self.mouselistener = MouseListener ()
        #OIS.MouseListener.__init_(self)
        
        self.Mouse.setEventCallback(self.mouselistener)
        self.Keyboard.setEventCallback(self)
        
        self.ShutdownRequested = False
        self.GUIRenderer = CEGUIRenderer
        self.keepRendering = True   # whether to continue rendering or not
        self.numScreenShots = 0     # screen shot count

    def _setupInput(self):
        pass
#         self.eventProcessor = ogre.EventProcessor()
#         self.eventProcessor.initialise(self.renderWindow)
#         self.eventProcessor.startProcessingEvents()

#         # register as a listener for events
#         self.eventProcessor.addKeyListener(self)
#         self.eventProcessor.addMouseListener(self)
#         self.eventProcessor.addMouseMotionListener(self)

    def frameStarted(self, evt):

        # after a few frameStarted calls the deadpool should be cleared 
        if not (cegui.WindowManager.getSingleton().isDeadPoolEmpty()):
            print "The dead Pool is Not Empty"
        return self.keepRendering

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
        self.ec = 0

    def _createGUI(self):

        # Initiaslise CEGUI Renderer
        self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow,ogre.RENDER_QUEUE_OVERLAY, False, 3000, self.sceneManager)
        self.system = cegui.System(self.guiRenderer)
        cegui.Logger.getSingleton().loggingLevel = cegui.Insane

        # Load Cegui Scheme
        cegui.SchemeManager.getSingleton().loadScheme("TaharezLookSkin.scheme")
        self.system.setDefaultMouseCursor ( "TaharezLook", "MouseArrow" )
        fm = cegui.FontManager.getSingleton()
        print "\n**", fm,"\n"
        #fm.createFont("Iconified-12.font")
        #cegui.FontManager.getSingleton().createFont("bluehighway-12.font")

        #  Layout Structure is:
        #  ----------------------
        #       Default.layout                         # loads GUI root window
        #       InfoBox.layout                         # has a frame/statictext/OK/Cancel button
        #            Name = "ExampleFrameWindow"
        #            Name = "ExampleStaticText"
        #            Name = "OK"
        #            Name = "Cancel"


        # Load Layout root window layout
        sheet = cegui.WindowManager.getSingleton().loadWindowLayout("Default.layout","","")
        self.system.guiSheet = sheet

        # Load our Frame.layout
        # Notice the optional window name prefix "Frame1/" and "Frame2/"
        # This allows us to load controls from the same layout with different names 
       
        fr1=cegui.WindowManager.getSingleton().loadWindowLayout("InfoBox.layout","Frame1/")
        fr2=cegui.WindowManager.getSingleton().loadWindowLayout("InfoBox.layout","Frame2/")
        fr3=cegui.WindowManager.getSingleton().loadWindowLayout("InfoBox.layout","Frame3/")

        # add layout controls to main window
        sheet.addChildWindow(fr1)
        sheet.addChildWindow(fr2)
        sheet.addChildWindow(fr3)

        # The controls may now be accessed via:
        # --------------------------------------
        #  Frame1/ExampleFrameWindow
        #  Frame1/ExampleStaticText
        #  Frame1/OK
        #  Frame1/Cancel
        #
        #  Frame2/ExampleFrameWindow
        #  Frame2/ExampleStaticText
        #  Frame2/OK
        #  Frame2/Cancel
        #
        # etc 
        #  -------------------------------------

        # Get controls
        frame1 = cegui.WindowManager.getSingleton().getWindow("Frame1/ExampleFrameWindow")
        frame1.text = "Info Box 1"
        frame2 = cegui.WindowManager.getSingleton().getWindow("Frame2/ExampleFrameWindow")
        frame2.text = "Info Box 2"
        uv= cegui.UVector2(cegui.UDim(1,0.5),cegui.UDim(1,0.5) ) #Point(0.5,0.5)    ?????
        frame2.position = uv
        textcontrol1 = cegui.WindowManager.getSingleton().getWindow("Frame1/ExampleStaticText")
        textcontrol2 = cegui.WindowManager.getSingleton().getWindow("Frame2/ExampleStaticText")
        textcontrol1.text = " Loading more than one instance layout"
        textcontrol2.text = " An Info widget"

        OKButton = cegui.WindowManager.getSingleton().getWindow("Frame3/OK")
#         self.ec = OKButton.subscribeEvent(OKButton.EventClicked, onButton)


        # delete one of the windows, remember to disconnect any events first  
#         self.ec.disconnect
        cegui.WindowManager.getSingleton().destroyWindow("Frame3/ExampleFrameWindow")

        # deadpool contains windows awaiting deletion
        #   -  true if there are no windows in the dead pool.
        #   -  false if the dead pool contains >=1 window awaiting destruction. 

        # should return not empty initially
        if (cegui.WindowManager.getSingleton().isDeadPoolEmpty()):
           print "The dead Pool is Empty"
        else:   
            print "The dead Pool is Not Empty"
      
        # That's it really 



	
    def _createScene(self):
        sceneManager = self.sceneManager
        sceneManager.ambientLight = ogre.ColourValue(0.25, 0.25, 0.25)
        self._createGUI()


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
