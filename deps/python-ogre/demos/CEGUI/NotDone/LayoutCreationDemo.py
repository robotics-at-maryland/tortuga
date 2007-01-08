# this code is in the public domain

#-----------------------------------------
#  Demo loading a TaharezLayout (Demo8.LAYOUT from gui\datafiles\layouts)
#
#  Same comments on :  
#     adding list items from within an event, cant see anyother way of doing it
#     Events still dont appear to be returning window etc
#
#
#----------------------------------------

import Ogre as ogre
import CEGUI as cegui
import OIS as OIS
#import ogre
#import cegui
import SampleFramework

import sys, exceptions, random

from CEGUI_framework import *

# Event Handling Add Button
def onAdd(args):
    #print `args`
    
    # Get the list box / edit box window
    editBox = cegui.WindowManager.getSingleton().getWindow("Demo8/Window1/Controls/Editbox")
    listBox = cegui.WindowManager.getSingleton().getWindow("Demo8/Window1/Listbox")
   
    # If there's text add to the listbox 
    txt=editBox.text    
    if (len(txt)>0):
        #txt = editBox.text
	#id = listBox.itemCount  
        item = cegui.ListboxTextItem(editBox.text,listBox.itemCount)

	# set the item selection colours and image otherwise you wont see the item
        item.selectionColours = cegui.colour(0.0, 0.0, 1.0, 1.0)
        item.selectionBrushImage = "TaharezLook", "ListboxSelectionBrush"
        # add item to list box
        listBox.addItem(item)
        item.thisown = 0
    editBox.text = ""

    
# Event Handling ColourBar Scroll
def onScrollUpdate(args):

    #print `args`
    # Hack cant seem to get window from event
    # get the scrollbar positions
    r = cegui.WindowManager.getSingleton().getWindow("Demo8/Window1/Controls/Red").scrollPosition
    g = cegui.WindowManager.getSingleton().getWindow("Demo8/Window1/Controls/Green").scrollPosition
    b = cegui.WindowManager.getSingleton().getWindow("Demo8/Window1/Controls/Blue").scrollPosition

    cegui.WindowManager.getSingleton().getWindow("Demo8/Window2/Tips").textColours = cegui.colour(r, g, b, 1.0)

    # Set the Static image colour
    cegui.WindowManager.getSingleton().getWindow("Demo8/Window1/Controls/ColourSample" ).imageColours = cegui.colour(r, g, b, 1.0)
    
# Event Handling Shows message mouse enters text box
def onMouseEnters(args):
    cegui.WindowManager.getSingleton().getWindow("Demo8/Window2/Tips").text="Hello"

def onMouseLeaves(args):
    cegui.WindowManager.getSingleton().getWindow("Demo8/Window2/Tips").text="Goodbye"

    
class GEUIApplication(SampleFramework.Application):

    def __init__(self):
        SampleFramework.Application.__init__(self)
        self.guiRenderer=0
        self.system =0

    def _createGUI(self):

       ## setup GUI system
 
        self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow,
                            ogre.RENDER_QUEUE_OVERLAY, False, 3000, self.sceneManager) 
#       self.guiRenderer.setTargetSceneManager(self.sceneManager)
        # Initiaslise CEGUI Renderer
        #self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow)
        self.system = cegui.System(self.guiRenderer)
        cegui.Logger.getSingleton().loggingLevel = cegui.Insane
        ## set the default resource groups to be used
        
        CEGUI.Imageset.setDefaultResourceGroup("imagesets")
        CEGUI.Font.setDefaultResourceGroup("fonts")
        CEGUI.Scheme.setDefaultResourceGroup("schemes")
        CEGUI.WidgetLookManager.setDefaultResourceGroup("looknfeels")
        CEGUI.WindowManager.setDefaultResourceGroup("layouts")
        CEGUI.ScriptModule.setDefaultResourceGroup("lua_scripts")

        # Load Cegui Scheme
        cegui.SchemeManager.getSingleton().loadScheme("WindowsLook.scheme")
        self.system.setDefaultMouseCursor ( "WindowsLook", "MouseArrow" )
        cegui.FontManager.getSingleton().createFont("Iconified-12.font")

        # Load Layout (from media/gui/datafiles/layout)
        sheet = cegui.WindowManager.getSingleton().loadWindowLayout("Demo8.layout")
        self.system.setGUISheet(sheet)

#         # Get controls
#         addButton = cegui.WindowManager.getSingleton().getWindow("Demo8/Window1/Controls/Add")
#         redScroll = cegui.WindowManager.getSingleton().getWindow("Demo8/Window1/Controls/Red")
#         greenScroll = cegui.WindowManager.getSingleton().getWindow("Demo8/Window1/Controls/Green")
#         blueScroll = cegui.WindowManager.getSingleton().getWindow("Demo8/Window1/Controls/Blue")
# 	    
#         # Info Panel
#         cegui.WindowManager.getSingleton().getWindow("Demo8/Window2/Info").text="An example of Text"

#         # Tips Window
#         cegui.WindowManager.getSingleton().getWindow("Demo8/Window2/Tips").text="Some Coloured Text"
#         
#         # Events
#         ec= addButton.subscribeEvent(addButton.EventClicked, onAdd, "")

        # -----------------------------------------------------------------------------         
        # Global Events, be careful with global events it employs late binding, i.e. 
        # you can define a global event for a Control/ControlEvent combination even
        # though that combination hasn't been yet created
        # -----------------------------------------------------------------------------         
        # create global event name i.e. "Scrollbar/ScrollPosChanged"	
#         global_event=redScroll.EventNamespace + "/" +  redScroll.EventScrollPositionChanged
#         ec2= cegui.GlobalEventSet.getSingleton().subscribeEvent(global_event, onScrollUpdate, "")
        
        # The above is equivalent 
        #ec2= redScroll.subscribeEvent(redScroll.EventScrollPositionChanged, onScrollUpdate)
        #ec3= greenScroll.subscribeEvent(greenScroll.EventScrollPositionChanged, onScrollUpdate)
        #ec4= blueScroll.subscribeEvent(blueScroll.EventScrollPositionChanged, onScrollUpdate)
	
    def _createScene(self):
        sceneManager = self.sceneManager
        sceneManager.ambientLight = ogre.ColourValue(0.25, 0.25, 0.25)
        self._createGUI()


    def _createCamera(self):
        self.camera = self.sceneManager.createCamera("PlayerCam")
        self.camera.nearClipDistance = 5

    def _createFrameListener(self):
        self.frameListener = GuiFrameListener(self.renderWindow, self.camera, self.guiRenderer)
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
