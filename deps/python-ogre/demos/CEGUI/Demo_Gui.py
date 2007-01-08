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
# */

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
        self.DescriptionMap={}
        self.ListItems = []
        
        # used for adding individual objects
        self.windowNumber = 0 
        self.vertScrollNumber = 0 
        self.horizScrollNumber = 0 
        self.textScrollNumber = 0 
        self.siCounter = 0 
        self.rttCounter=0

 
        self.DescriptionMap[ "Demo8"] = "The main containing panel" 
        self.DescriptionMap[ "Demo8/Window1"] =  "A test window" 
        self.DescriptionMap[ "Demo8/Window1/Listbox"] = "A list box" 
        self.DescriptionMap[ "Demo8/Window1/Controls/Red"] =  "A colour slider" 
        self.DescriptionMap[ "Demo8/Window1/Controls/Green"] = "A colour slider" 
        self.DescriptionMap[ "Demo8/Window1/Controls/Blue"] =  "A colour slider" 
        self.DescriptionMap[ "Demo8/Window1/Controls/ColourSample"] =  "The colour that will be used for the selection when added to the list" 
        self.DescriptionMap[ "Demo8/Window1/Controls/Editbox"] =  "An edit box  self text will be added to the list" 
        self.DescriptionMap[ "Demo8/Window1/Controls/Add"] =  "Adds the text to the list" 
        self.DescriptionMap[ "Demo8/Window1/Controls/ins1"] =  "Some static text" 
    

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
        ## Accept default settings: point light, white diffuse, just set position
        ## NB I could attach the light to a SceneNode if I wanted it to move automatically with
        ##  other objects, but I don't
        l.setPosition(20,80,50) 

        ## setup GUI system
        self.GUIRenderer = CEGUI.OgreCEGUIRenderer(self.renderWindow, 
            ogre.RENDER_QUEUE_OVERLAY, False, 3000, self.sceneManager) 

            
        self.GUIsystem = CEGUI.System(self.GUIRenderer) 

        logger = CEGUI.Logger.getSingleton()
        level = CEGUI.Informative
        logger.setLoggingLevel(level) 

        ogreHead = self.sceneManager.createEntity("Head", "ogrehead.mesh") 

        headNode = self.sceneManager.getRootSceneNode().createChildSceneNode() 
        headNode.attachObject(ogreHead) 


        ## Setup Render To Texture for preview window
        rttTex = self.root.getRenderSystem().createRenderTexture( "RttTex", 512, 512, ogre.TextureType.TEX_TYPE_2D, ogre.PixelFormat.PF_R8G8B8 ) 
        rttCam = self.sceneManager.createCamera("RttCam") 
        camNode = self.sceneManager.getRootSceneNode().createChildSceneNode("rttCamNode") 
        camNode.attachObject(rttCam) 
        rttCam.setPosition(0,0,200) 
        ##rttCam.setVisible(True) 
        v = rttTex.addViewport( rttCam ) 
        v.setOverlaysEnabled(False) 
        v.setClearEveryFrame( True ) 
        v.setBackgroundColour( ogre.ColourValue.Black ) 

            
        ## Retrieve CEGUI texture for the RTT
        rttTexture = self.GUIRenderer.createTexture( "RttTex") 

        rttImageSet = CEGUI.ImagesetManager.getSingleton().createImageset("RttImageset", rttTexture) 

        rttImageSet.defineImage( "RttImage", 
                CEGUI.Point(0.0, 0.0),
                CEGUI.Size(rttTexture.getWidth(), rttTexture.getHeight()),
                CEGUI.Point(0.0,0.0)) 

        ## load scheme and set up defaults
        CEGUI.SchemeManager.getSingleton().loadScheme("TaharezLookSkin.scheme") 
        self.GUIsystem.setDefaultMouseCursor("TaharezLook",  "MouseArrow") 
        self.GUIsystem.setDefaultFont( "BlueHighway-12") 
        sheet = CEGUI.WindowManager.getSingleton().loadWindowLayout("ogregui.layout")  
        self.GUIsystem.setGUISheet(sheet) 
        
## note that copies are not made for combo/list boxes so need to keep each item arround
#         CEGUI.Testit()
        objectComboBox = CEGUI.WindowManager.getSingleton().getWindow("OgreGuiDemo/TabCtrl/Page2/ObjectTypeList") 
        self.item = CEGUI.ListboxTextItem( "FrameWindow", 0) 
        objectComboBox.addItem(self.item) 
        self.item1 = CEGUI.ListboxTextItem( "Horizontal Scrollbar", 1) 
        objectComboBox.addItem(self.item1) 
        self.item2 = CEGUI.ListboxTextItem( "Vertical Scrollbar", 2) 
        objectComboBox.addItem(self.item2) 
        self.item3 = CEGUI.ListboxTextItem( "StaticText", 3) 
        objectComboBox.addItem(self.item3) 
        self.item4 = CEGUI.ListboxTextItem( "StaticImage", 4) 
        objectComboBox.addItem(self.item4) 
        self.item5 = CEGUI.ListboxTextItem( "Render to Texture", 5) 
        objectComboBox.addItem(self.item5) 
        
        self.setupEventHandlers() 
        
        
    ## Create new frame listener
    def _createFrameListener(self):
        self.frameListener = GuiFrameListener(self.renderWindow, self.camera, self.GUIRenderer) #self.sceneManager)
        self.frameListener.MenuMode = True # this stop the camera moving with the mouse cursor..

        self.root.addFrameListener(self.frameListener)
        self.frameListener.showDebugOverlay(False)


    def setupEventHandlers(self):
        wmgr = CEGUI.WindowManager.getSingleton() 
        
        wmgr.getWindow( "OgreGuiDemo/TabCtrl/Page1/QuitButton").subscribeEvent(
                            CEGUI.PushButton.EventClicked, self, "handleQuit")
        wmgr.getWindow( "OgreGuiDemo/TabCtrl/Page1/NewButton").subscribeEvent(
                 CEGUI.PushButton.EventClicked, self, "handleNew") 
        wmgr.getWindow( "OgreGuiDemo/TabCtrl/Page1/LoadButton").subscribeEvent(
                CEGUI.PushButton.EventClicked, self, "handleLoad") 
        wmgr.getWindow("OgreGuiDemo/TabCtrl/Page2/ObjectTypeList").subscribeEvent(
				CEGUI.Combobox.EventListSelectionAccepted, self, "handleObjectSelection");

				                
    def setupEnterExitEvents(self, win):
        win.subscribeEvent(
            CEGUI.Window.EventMouseEnters, 
            self, "handleMouseEnters") 
        win.subscribeEvent(
            CEGUI.Window.EventMouseLeaves, 
            self, "handleMouseLeaves") 
        for i in range (win.getChildCount()):
            child = win.getChildAtIdx(i) 
            self.setupEnterExitEvents(child) 

    def setupLoadedLayoutHandlers(self):
        wmgr = CEGUI.WindowManager.getSingleton() 
        self.Red = wmgr.getWindow( "Demo8/Window1/Controls/Red")
        self.Green = wmgr.getWindow( "Demo8/Window1/Controls/Green") 
        self.Blue = wmgr.getWindow( "Demo8/Window1/Controls/Blue") 
        self.Preview = wmgr.getWindow( "Demo8/Window1/Controls/ColourSample") 
        self.List = wmgr.getWindow( "Demo8/Window1/Listbox") 
        self.EditBox = wmgr.getWindow( "Demo8/Window1/Controls/Editbox") 
        self.Tip = wmgr.getWindow( "Demo8/Window2/Tips") 

        self.Red.subscribeEvent(
              CEGUI.Scrollbar.EventScrollPositionChanged, 
              self, "handleColourChanged") 
        self.Green.subscribeEvent(
              CEGUI.Scrollbar.EventScrollPositionChanged, 
              self, "handleColourChanged") 
        self.Blue.subscribeEvent(
              CEGUI.Scrollbar.EventScrollPositionChanged, 
              self, "handleColourChanged") 
        wmgr.getWindow( "Demo8/Window1/Controls/Add").subscribeEvent(
              CEGUI.PushButton.EventClicked, 
              self, "handleAdd") 
               
        root = wmgr.getWindow("Demo8") 
        self.setupEnterExitEvents(root) 
      
    def createRttGuiObject(self):
        guiObjectName = "NewRttImage" + str(self.rttCounter) 
        
        rttImageSet = CEGUI.ImagesetManager.getSingleton().getImageset("RttImageset") 
        si = CEGUI.WindowManager.getSingleton().createWindow( "TaharezLook/StaticImage",  guiObjectName) 
        si.setSize(CEGUI.UVector2( CEGUI.UDim(0.5, 0), CEGUI.UDim(0.4, 0))) 
        si.setProperty("Image", CEGUI.PropertyHelper.imageToString(rttImageSet.getImage( "RttImage"))) 
        self.rttCounter += 1 
        return si 

    def createStaticImageObject(self):
        guiObjectName = "NewStaticImage" + str(self.siCounter) 

        imageSet = CEGUI.ImagesetManager.getSingleton().getImageset("TaharezLook") 

        si = CEGUI.WindowManager.getSingleton().createWindow( "TaharezLook/StaticImage",  guiObjectName) 
        si.setSize(CEGUI.UVector2( CEGUI.UDim(0.2, 0), CEGUI.UDim(0.2, 0))) 
        si.setProperty("Image", CEGUI.PropertyHelper.imageToString(imageSet.getImage( "ClientBrush"))) 

        self.siCounter+=1
        return si 

    def handleQuit(self, e):
        self.frameListener.requestShutdown() 
        return True

        
    def handleNew(self,  e):
        if self.EditorGuiSheet:
            CEGUI.WindowManager.getSingleton().destroyWindow(self.EditorGuiSheet) 
        
        self.EditorGuiSheet = CEGUI.WindowManager.getSingleton().createWindow("DefaultGUISheet", "NewLayout") 
        
        editorWindow = CEGUI.WindowManager.getSingleton().getWindow( "OgreGuiDemo2/MainWindow")
        editorWindow.addChildWindow( self.EditorGuiSheet ) 
        return True

        
    def handleLoad(self, e):
        if(self.EditorGuiSheet):
            CEGUI.WindowManager.getSingleton().destroyWindow(self.EditorGuiSheet) 
    
        self.EditorGuiSheet = CEGUI.WindowManager.getSingleton().loadWindowLayout("cegui8.layout")  
        self.setupLoadedLayoutHandlers() 
    
        editorWindow = CEGUI.WindowManager.getSingleton().getWindow( "OgreGuiDemo2/MainWindow") 
        editorWindow.addChildWindow(self.EditorGuiSheet) 
    
        return True


    def handleObjectSelection(self, e):
        guiObjectName =""
     
        ## Set a random position to place self object.
        posX = 0.7 * random.random() #(0.0, 0.7)  
        posY = 0.6 * random.random()  + 0.1 #(0.1, 0.7)  
        item = e.window.getSelectedItem() #CEGUI.DoIt(e)

        editorWindow = CEGUI.WindowManager.getSingleton().getWindow( "OgreGuiDemo2/MainWindow") 

        id = item.getID()
        if id == 0:
            guiObjectName = "NewWindow" + str(self.windowNumber) 
            window = CEGUI.WindowManager.getSingleton().createWindow( "TaharezLook/FrameWindow",  guiObjectName) 
            window.setSize(CEGUI.UVector2(CEGUI.UDim(0.3,0), CEGUI.UDim(0.3,0))) 
            window.setText( "New Window") 
            self.windowNumber+=1 
        elif id == 1:
            guiObjectName = "NewHorizScroll" + str(self.horizScrollNumber) 
            window = CEGUI.WindowManager.getSingleton().createWindow( "TaharezLook/HorizontalScrollbar",  guiObjectName) 
            window.setSize(CEGUI.UVector2(CEGUI.UDim(0.75,0), CEGUI.UDim(0.03,0))) 
            self.horizScrollNumber+=1 
        elif id == 2:
            guiObjectName = "NewVertScroll" + str(self.vertScrollNumber) 
            window = CEGUI.WindowManager.getSingleton().createWindow( "TaharezLook/VerticalScrollbar",  guiObjectName) 
            window.setSize(CEGUI.UVector2(CEGUI.UDim(0.03,0), CEGUI.UDim(0.75,0))) 
            self.vertScrollNumber+=1
        elif id == 3:
            guiObjectName = "NewStaticText" + str(self.textScrollNumber) 
            window = CEGUI.WindowManager.getSingleton().createWindow( "TaharezLook/StaticText",  guiObjectName) 
            window.setSize(CEGUI.UVector2(CEGUI.UDim(0.25,0), CEGUI.UDim(0.1,0))) 
            window.setText( "Example static text") 
            self.textScrollNumber+=1 
        elif id == 4:
            window = self.createStaticImageObject() 
        elif id == 5:
            window = self.createRttGuiObject() 

        editorWindow.addChildWindow(window) 
        window.setPosition(CEGUI.UVector2(CEGUI.UDim(posX, 0), CEGUI.UDim(posY, 0))) 

        return True

    def handleColourChanged( self, e):
        self.Preview.setProperty("ImageColours",
            CEGUI.PropertyHelper.colourToString(CEGUI.colour(
                self.Red.getScrollPosition() / 255.0,
                self.Green.getScrollPosition() / 255.0,
                self.Blue.getScrollPosition() / 255.0))) 
        
        return True

    def handleAdd(self,  e):
        listboxitem = CEGUI.ListboxTextItem (self.EditBox.getText()) 
        listboxitem.setSelectionBrushImage("TaharezLook", "ListboxSelectionBrush") 
        if self.List.getItemCount() == 0:
            listboxitem.setSelected( True ) 
        else:
            listboxitem.setSelected( False ) 
        listboxitem.setSelectionColours(
            CEGUI.PropertyHelper.stringToColourRect(self.Preview.getProperty("ImageColours"))) 
        self.List.addItem(listboxitem)
        self.ListItems.append(listboxitem) # we need to keep the listitems around for the list box to work
        return True
 
    def handleMouseEnters(self,  e):
        name = e.window.getName().c_str()
        try:
            self.Tip.setText( self.DescriptionMap[ name ])
        except:
            self.Tip.setText( "" )
        return True
        
    def handleMouseLeaves(self, e):
        self.Tip.setText( "") 
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
    
        

