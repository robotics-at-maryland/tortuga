# /*
# -----------------------------------------------------------------------------
# This source file is part of OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http:##www.ogre3d.org/

# Copyright (c) 2000-2006 Torus Knot Software Ltd
# Also see acknowledgements in Readme.html

# You may use self sample code for anything you like, it is not covered by the
# LGPL like the rest of the engine.
# -----------------------------------------------------------------------------
# 

###
###  This is a blank template to make it easier to convert existing samples
###

import Ogre as ogre
import CEGUI as CEGUI
import OIS as OIS
import SampleFramework

import sys, exceptions, random

from CEGUI_framework import *   ## we need the OIS version of the framelistener etc

class GuiApplication ( SampleFramework.Application ):
   
    def __init__(self):
        SampleFramework.Application.__init__(self)
        self.GUIRenderer=0
        self.GUIsystem =0
        self.EditorGuiSheet=0
        
    def __del__(self):
        ##
        ## important that things get deleted int he right order
        ##
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
   
 
    ## Just override the mandatory create scene method
    def _createScene(self):
        sceneManager = self.sceneManager
        sceneManager.ambientLight = ogre.ColourValue(0.5, 0.5, 0.5)

        ## Create a skydome
        self.sceneManager.setSkyDome(True, "Examples/CloudySky", 5, 8) 

        ## Create a light
        l = self.sceneManager.createLight("MainLight") 
        l.setPosition(20,80,50) 

        ## setup GUI system
        self.GUIRenderer = CEGUI.OgreCEGUIRenderer(self.renderWindow, 
            ogre.RENDER_QUEUE_OVERLAY, False, 3000, self.sceneManager) 

        ## set the default resource groups to be used
        CEGUI.Imageset.setDefaultResourceGroup("imagesets")
        CEGUI.Font.setDefaultResourceGroup("fonts")
        CEGUI.Scheme.setDefaultResourceGroup("schemes")
        CEGUI.WidgetLookManager.setDefaultResourceGroup("looknfeels")
        CEGUI.WindowManager.setDefaultResourceGroup("layouts")
        CEGUI.ScriptModule.setDefaultResourceGroup("lua_scripts")

                    
        self.GUIsystem = CEGUI.System(self.GUIRenderer) 

        logger = CEGUI.Logger.getSingleton()
        logger.setLoggingLevel( CEGUI.Informative ) 

        ## load scheme and set up defaults
        CEGUI.SchemeManager.getSingleton().loadScheme("TaharezLook.scheme") 
        self.GUIsystem.setDefaultMouseCursor("TaharezLook",  "MouseArrow") 
        #self.GUIsystem.setDefaultFont( "BlueHighway-12") 
        CEGUI.FontManager.getSingleton().createFont("Commonwealth-10.font")
        self.GUIsystem.setDefaultFont( "Commonwealth-10") 
        #sheet = CEGUI.WindowManager.getSingleton().loadWindowLayout("ogregui.layout")  
        ## load an image to use as a background
        CEGUI.ImagesetManager.getSingleton().createImagesetFromImageFile("BackgroundImage", "GPN-2000-001437.tga")
    
        ## here we will use a StaticImage as the root, then we can use it to place a background image
        background = CEGUI.WindowManager.getSingleton().createWindow("TaharezLook/StaticImage", "background_wnd")
        ## set position and size
        background.setPosition(CEGUI.UVector2(cegui_reldim(0), cegui_reldim( 0)))
        background.setSize(CEGUI.UVector2(cegui_reldim(1), cegui_reldim( 1)))
        ## disable frame and standard background
        background.setProperty("FrameEnabled", "false")
        background.setProperty("BackgroundEnabled", "false")
        ## set the background image
        background.setProperty("Image", "set:BackgroundImage image:full_image")
        ## install this as the root GUI sheet
        self.GUIsystem.setGUISheet(background) 
	self.lb=self.CreateComponent("TaharezLook/Listbox","TestBox",(0,0),
	  (0.5,0.5),(0.5,0.5),(0.5,0.5))
        background.addChildWindow(self.lb)

	print "Reset List before Add... ",
	self.lb.resetList()
	print "Ok"
	print "Adding Item... ",
	self.lbi=self.AddItemToListBox(self.lb, "Test test test")
	print "Ok"

	print "Reset List after Add... ",
	self.lb.resetList()
	print "Ok"

        ## now setup any event handlers you want       
        self.setupEventHandlers() 
        
    def AddItemToListBox(self, l, t):
      import CEGUI as cegui
      item=cegui.ListboxTextItem(t,l.itemCount+1)
      item.setSelectionBrushImage( "TaharezLook", "ListboxSelectionBrush")
      item.setSelectionColours (cegui.colour(1.0, 0.0, 0.0, 1.0))
      item.setTextColours (cegui.colour(0.0, 0.0, 1.0, 1.0))
      item.setAutoDeleted=False
      l.addItem(item)
      return item        

    def CreateComponent(self,widget_name,name,pos,size,min,max,text='',showframe=False):
      """ Manually create a component! """
      import CEGUI as cegui
      control = cegui.WindowManager.getSingleton().createWindow(widget_name, name)
      control.position = cegui.UVector2(cegui.UDim(pos[0],0),cegui.UDim(pos[1],0))
      control.setSize(cegui.UVector2(cegui.UDim(size[0],0),cegui.UDim(size[1],0)))
      control.setText( text)
      control.frameEnabled= showframe
      return control

    ## Create new frame listener
    def _createFrameListener(self):
        self.frameListener = GuiFrameListener(self.renderWindow, self.camera, self.GUIRenderer) #self.sceneManager)
        self.root.addFrameListener(self.frameListener)
        self.frameListener.showDebugOverlay(False)


    def setupEventHandlers(self):
        wmgr = CEGUI.WindowManager.getSingleton() 
        
#         wmgr.getWindow( "OgreGuiDemo/TabCtrl/Page1/QuitButton").subscribeEvent(
#                             CEGUI.PushButton.EventClicked, self, "handleQuit")
#         wmgr.getWindow("OgreGuiDemo/TabCtrl/Page2/ObjectTypeList").subscribeEvent(
# 				CEGUI.Combobox.EventListSelectionAccepted, self, "handleObjectSelection");

				                
    def handleQuit(self, e):
        self.frameListener.requestShutdown() 
        return True
 
if __name__ == '__main__':
    try:
        ta = GuiApplication()
        ta.go()
    except ogre.Exception, e:
        print e
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
    
        

