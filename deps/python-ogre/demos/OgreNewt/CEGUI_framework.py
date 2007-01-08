import CEGUI as CEGUI
import OIS as OIS
import Ogre as Ogre
import SampleFramework

##----------------------------------------------------------------##
def convertOISMouseButtonToCegui( buttonID):
    if buttonID ==0:
        return CEGUI.LeftButton
    elif buttonID ==1:
        return CEGUI.RightButton
    elif buttonID ==2:
        return CEGUI.MiddleButton
    elif buttonID ==3:
        return CEGUI.X1Button
    else:
        return CEGUI.LeftButton

def _PointHack(x, y):
    return CEGUI.Vector2(x, y)
CEGUI.Point = _PointHack

def cegui_reldim ( x ) :
    return CEGUI.UDim((x),0)
    
class MouseListener ( OIS.MouseListener ):
    def __init__(self):
        OIS.MouseListener.__init__( self)

    ##----------------------------------------------------------------##
    def mouseMoved( self, arg ):
        CEGUI.System.getSingleton().injectMouseMove( arg.get_state().X.rel, arg.get_state().Y.rel )
        return True

    ##----------------------------------------------------------------##
    def mousePressed(  self, arg, id ):
        CEGUI.System.getSingleton().injectMouseButtonDown(convertOISMouseButtonToCegui(id))
        return True

    ##----------------------------------------------------------------##
    def mouseReleased( self, arg, id ):
        CEGUI.System.getSingleton().injectMouseButtonUp(convertOISMouseButtonToCegui(id))
        return True

        
class GuiFrameListener(SampleFramework.FrameListener, OIS.KeyListener, OIS.MouseListener):

    def __init__(self, renderWindow, camera,  CEGUIRenderer):

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

    ## Tell the frame listener to exit at the end of the next frame
    def requestShutdown( self ):
        self.ShutdownRequested = True

    def frameEnded(self, evt):
        if self.ShutdownRequested:
            return False
        else:
            return SampleFramework.FrameListener.frameEnded(self, evt)
        
    ##----------------------------------------------------------------##
    def keyPressed( self, arg ):
        if arg.key == OIS.KC_ESCAPE:
            self.ShutdownRequested = True
        CEGUI.System.getSingleton().injectKeyDown( arg.key )
        CEGUI.System.getSingleton().injectChar( arg.text )
        return True

    ##----------------------------------------------------------------##
    def keyReleased( self, arg ):
        CEGUI.System.getSingleton().injectKeyUp( arg.key )
        return True
