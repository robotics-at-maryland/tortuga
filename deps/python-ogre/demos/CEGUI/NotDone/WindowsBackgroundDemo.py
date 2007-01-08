# this code is in the public domain
#
#    This demo shows how to create an image background
#    Demo using WindowsLook
#    This demo is based on the original Cegui FalagardDemo1 by Paul D Turner
#
# 

from pyogre import ogre,cegui

import SampleFramework


def _PointHack(x, y):
    return cegui.Vector2(x, y)
cegui.Point = _PointHack


class CEGUIFrameListener(SampleFramework.FrameListener):

    def __init__(self, renderWindow, camera, sceneManager):

        SampleFramework.FrameListener.__init__(self, renderWindow, camera)

        self.keepRendering = True   # whether to continue rendering or not
        self.numScreenShots = 0     # screen shot count


    def _setupInput(self):
        self.eventProcessor = ogre.EventProcessor()
        self.eventProcessor.initialise(self.renderWindow)
        self.eventProcessor.startProcessingEvents()

        # register as a listener for events
        self.eventProcessor.addKeyListener(self)
        self.eventProcessor.addMouseListener(self)
        self.eventProcessor.addMouseMotionListener(self)


    def frameStarted(self, evt):
        # needed for tooltip fading etc
       	cegui.System.getSingleton().injectTimePulse( evt.timeSinceLastFrame)
        return self.keepRendering

    def mouseDragged(self, evt):
        self.mouseMoved(evt)

    def mousePressed(self, evt):
        button = self._convertOgreButtonToCegui(evt)
        # simple mouse in window event handling without connection 
        win_mouse = cegui.System.getSingleton().windowContainingMouse
        if (win_mouse):
	    if (win_mouse.name=="Demo/Print"):
                self.onPrint()

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

    # print screen 
    def onPrint(self):
        path = 'screenshot_%d.png' % self.numScreenShots
        self.numScreenShots += 1
        self.renderWindow.writeContentsToFile(path)
        self.renderWindow.debugText = 'screenshot taken: ' + path


# Event Handling for image selection 
def onShowImage(args):

    # get the image name list box
    listBox = cegui.WindowManager.getSingleton().getWindow("Demo7/Window2/Listbox")
    addButton = cegui.WindowManager.getSingleton().getWindow("Demo/NewNode/Okay")
    try:
        # list box has items
        if (listBox.itemCount):
            # get the tga filename 
            txt =listBox.getFirstSelectedItem().text
            # set static text/image set static label to name
            cegui.WindowManager.getSingleton().getWindow("Demo/ImageViewer/area1").text=txt
            staticImage = cegui.WindowManager.getSingleton().getWindow("Demo/ImageViewer/area2")

            # If imageset already present delete and recreate
            if cegui.ImagesetManager.getSingleton().isImagesetPresent("BackgroundImage2"):
                cegui.ImagesetManager.getSingleton().destroyImageset("BackgroundImage2")
                # create imageset / image from texture
                cegui.ImagesetManager.getSingleton().createImagesetFromImageFile("BackgroundImage2", txt)
                staticImage.image=("BackgroundImage2", "full_image")
                addButton.setProperty("NormalImage","set: BackgroundImage2 image: full_image" ) 

    except: 
        print "Event Error"
    #std::string strProperty = "set:" + strImageSet + " image:" + strNormalPic;
    #pushBtn->setProperty("NormalImage",strProperty);
		


class GEUIApplication(SampleFramework.Application):

    def __init__(self):
        SampleFramework.Application.__init__(self)
        self.guiRenderer=0
        self.system =0
        self.img = []    # static images for frames
        self.ec=0

    def _createGUI(self):

        # initiaslise CEGUI Renderer
        self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow,ogre.RENDER_QUEUE_OVERLAY, False, self.sceneManager)
        self.system = cegui.System(self.guiRenderer)
        cegui.Logger.getSingleton().loggingLevel = cegui.Insane

        # load Cegui WindowsLook Skin Scheme
        cegui.SchemeManager.getSingleton().loadScheme("WindowsLookSkin.scheme")
        self.system.defaultMouseCursor=("WindowsLook", "MouseArrow")

        # load an image to use as a background
        cegui.ImagesetManager.getSingleton().createImagesetFromImageFile("BackgroundImage", "GPN-2000-001437.tga")

        # here we will use a StaticImage as the root and use it to place a background image
        background = cegui.WindowManager.getSingleton().createWindow("WindowsLook/StaticImage", "BackgroundImage")
 
        # set area rectangle
        background.rect=(cegui.Relative, cegui.Rect(0, 0, 1, 1))
        background.setFormatting(background.HorzStretched,background.VertStretched)

        background.frameEnabled =False      # disable frame and standard background
        background.backgroundEnabled=False  # set the background image

        background.image=("BackgroundImage", "full_image")
        # install this as the root GUI sheet
        cegui.System.getSingleton().guiSheet=background

        cegui.FontManager.getSingleton().createFont("Iconified-12.font")

        # load some demo windows and attach to the background 'root'
        background.addChildWindow(cegui.WindowManager.getSingleton().loadWindowLayout("WindowsLookBackground.layout"))

        background.activate()
        
        # load ogre tga textures in listbox  
        red  = cegui.colour(1.0, 0.0, 0.0, 0.5)
	listbox = cegui.WindowManager.getSingleton().getWindow("Demo7/Window2/Listbox")
        imageList = ogre.ResourceGroupManager.getSingleton().findResourceNames("General", "*.png")
        iterator = imageList.get()
        images = [ iterator.__getitem__(j) for j in range(iterator.size()) ]
        black  = cegui.colour(0.0, 0.0, 0.0, 1.0)
        for i, v in enumerate(images):
            item = cegui.ListboxTextItem(v,i)
            self.img.append(item)
            item.selectionColours=red
            item.textColours=black
            item.selectionBrushImage=("WindowsLook", "Background")
            listbox.addItem(item)

        # set image viewer characteristics
        frame = cegui.WindowManager.getSingleton().getWindow("Demo/ImageViewer")
        frame.alpha = 0.8
        staticimage = cegui.WindowManager.getSingleton().getWindow("Demo/ImageViewer/area2")
        staticimage.frameColours=(cegui.colour(0.0, 1.0, 0.0, 1.0))  
        staticimage.frameEnabled = True  

        # set image list frame characteristics
        frame = cegui.WindowManager.getSingleton().getWindow("Demo/NewNode")
        frame.alpha = 0.8

        # set image viewer characteristics
        cegui.ImagesetManager.getSingleton().createImagesetFromImageFile("BackgroundImage2", "GPN-2000-001437.tga")
        staticimage.image=("BackgroundImage2", "full_image")
       
        # suscribe show image event
        addButton = cegui.WindowManager.getSingleton().getWindow("Demo/NewNode/Okay")
        addButton.alpha = 0.5
        self.ec = addButton.subscribeEvent(str(addButton.EventClicked), onShowImage)


    def _createScene(self):
        sceneManager = self.sceneManager
        sceneManager.ambientLight = 0.25, 0.25, 0.25
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
