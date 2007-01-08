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
        CEGUI.SchemeManager.getSingleton().loadScheme("TaharezLookSkin.scheme") 
        self.GUIsystem.setDefaultMouseCursor("TaharezLook",  "MouseArrow") 
        self.GUIsystem.setDefaultFont( "BlueHighway-12") 
        sheet = CEGUI.WindowManager.getSingleton().loadWindowLayout("ogregui.layout")  
        self.GUIsystem.setGUISheet(sheet) 
                
        ## now setup any event handlers you want       
        self.setupEventHandlers() 
        
        
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
    
        

