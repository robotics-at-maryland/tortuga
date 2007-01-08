# this code is in the public domain
#
#    This demo shows how to create an image background
#    Demo using Falgard - VanillaSkin scheme
#    This demo is based on the original Cegui FalagardDemo1 by Paul D Turner
#
# 


import Ogre as ogre
import CEGUI as cegui
import OIS as OIS
import SampleFramework

from CEGUI_framework import *


# Event Handling for image selection 
def onShowImage(args):

    # get the image name list box
    listBox = cegui.WindowManager.getSingleton().getWindow("Demo7/Window2/Listbox")
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

    except: 
        print "Event Error"


class GEUIApplication(SampleFramework.Application):

    def __init__(self):
        SampleFramework.Application.__init__(self)
        self.guiRenderer=0
        self.system =0
        self.img = []    # static images for frames
        self.ec=0

    def _createGUI(self):

        # initiaslise CEGUI Renderer
        # AJM Change in call parameters, see the 0 whihc is maxquads

        
        self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow,ogre.RENDER_QUEUE_OVERLAY, False, 3000,self.sceneManager)      
        self.system = cegui.System(self.guiRenderer)
 
        cegui.Logger.getSingleton().setLoggingLevel(cegui.Informative) #loggingLevel = cegui.Insane

        # load Cegui Vanilla Skin Scheme
        cegui.SchemeManager.getSingleton().loadScheme(scheme_filename="TaharezLookSkin.scheme")
        self.system.setDefaultMouseCursor("TaharezLook", "MouseArrow")

        # load an image to use as a background
        cegui.ImagesetManager.getSingleton().createImagesetFromImageFile("BackgroundImage", "GPN-2000-001437.tga")

        # here we will use a StaticImage as the root and use it to place a background image
#        background = cegui.WindowManager.getSingleton().createWindow("", "BackgroundImage")
        background = cegui.WindowManager.getSingleton().createWindow("Vanilla/StaticImage", "BackgroundImage")
 
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
        background.addChildWindow(cegui.WindowManager.getSingleton().loadWindowLayout("DemoVanillaSkin.layout"))

        background.activate()
        
        # load ogre tga textures in listbox  
        red  = cegui.colour(1.0, 0.0, 0.0, 0.5)
        listbox = cegui.WindowManager.getSingleton().getWindow("Demo7/Window2/Listbox")
        imageList = ogre.ResourceGroupManager.getSingleton().findResourceNames("General", "*.tga")
        iterator = imageList.get()
        images = [ iterator.__getitem__(j) for j in range(iterator.size()) ]
        for i, v in enumerate(images):
            self.img.append(cegui.ListboxTextItem(v,i))
            item = self.img[i]
            item.selectionColours=red
            item.selectionBrushImage=("Vanilla-Images", "GenericBrush")
            listbox.addItem(item)

        # set image viewer characteristics
        frame = cegui.WindowManager.getSingleton().getWindow("Demo/ImageViewer")
        frame.alpha = 0.5
        staticimage = cegui.WindowManager.getSingleton().getWindow("Demo/ImageViewer/area2")
        staticimage.frameColours=(cegui.colour(0.0, 1.0, 0.0, 1.0))  
        staticimage.frameEnabled = True  

        # set image viewer characteristics
        cegui.ImagesetManager.getSingleton().createImagesetFromImageFile("BackgroundImage2", "GPN-2000-001437.tga")
        staticimage.image=("BackgroundImage2", "full_image")
       
        # suscribe show image event
        addButton = cegui.WindowManager.getSingleton().getWindow("Demo/NewNode/Okay")
        addButton.alpha = 0.5
        self.ec = addButton.subscribeEvent(cegui.PushButton.EventClicked, onShowImage,"")


    def _createScene(self):
        sceneManager = self.sceneManager
        sceneManager.ambientLight = ogre.ColourValue(0.25, 0.25, 0.25)
        self._createGUI()


    def _createCamera(self):
        self.camera = self.sceneManager.createCamera("PlayerCam")
        self.camera.nearClipDistance = 5

    def _createFrameListener(self):
        self.frameListener = GuiFrameListener(self.renderWindow, self.camera, self.sceneManager)
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
    except ogre.Exception, e:
        print e
        
