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

def cegui_reldim ( x ) :
    return CEGUI.UDim((x),0)

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

#         ## set the default resource groups to be used
#         CEGUI.Imageset.setDefaultResourceGroup("imagesets")
#         CEGUI.Font.setDefaultResourceGroup("fonts")
#         CEGUI.Scheme.setDefaultResourceGroup("schemes")
#         CEGUI.WidgetLookManager.setDefaultResourceGroup("looknfeels")
#         CEGUI.WindowManager.setDefaultResourceGroup("layouts")
#         CEGUI.ScriptModule.setDefaultResourceGroup("lua_scripts")
#         
        ## setup GUI system
        self.GUIRenderer = CEGUI.OgreCEGUIRenderer(self.renderWindow, 
            ogre.RENDER_QUEUE_OVERLAY, False, 3000, self.sceneManager) 

            
        self.GUIsystem = CEGUI.System(self.GUIRenderer) 
   
        logger = CEGUI.Logger.getSingleton()
        logger.setLoggingLevel( CEGUI.Informative ) 

        # we will make extensive use of the WindowManager.
        winMgr = CEGUI.WindowManager.getSingleton()

        ## load scheme and set up defaults
        CEGUI.SchemeManager.getSingleton().loadScheme("TaharezLook.scheme") 
        self.GUIsystem.setDefaultMouseCursor("TaharezLook",  "MouseArrow") 
        CEGUI.FontManager.getSingleton().createFont("Commonwealth-10.font")

        ## load an image to use as a background
        CEGUI.ImagesetManager.getSingleton().createImagesetFromImageFile("BackgroundImage", "GPN-2000-001437.tga")
        
        ## here we will use a StaticImage as the root, then we can use it to place a background image
        background = winMgr.createWindow("TaharezLook/StaticImage", "background_wnd")
        ## set position and size
        background.setPosition(CEGUI.UVector2(cegui_reldim(0), cegui_reldim( 0)))
        background.setSize(CEGUI.UVector2(cegui_reldim(1), cegui_reldim( 1)))
        ## disable frame and standard background
        background.setProperty("FrameEnabled", "false")
        background.setProperty("BackgroundEnabled", "false")
        ## set the background image
        background.setProperty("Image", "set:BackgroundImage image:full_image")
        ## install this as the root GUI sheet
        CEGUI.System.getSingleton().setGUISheet(background)
            
        ## now we create a DefaultWindow which we will attach all the widgets to.  We could
        ## have attached them to the background StaticImage, though we want to be a bit tricky
        ## since we do not wish the background to be faded by the slider - so we create this
        ## container window so we can affect all the other widgets, but leave the background
        ## unchanged.
        sheet = winMgr.createWindow("DefaultWindow", "root_wnd")
        ## attach this to the 'real' root
        background.addChildWindow(sheet)
        
        ##
        ## widget initialisation
        ##
        ## quit button
        btn = winMgr.createWindow("TaharezLook/Button", "QuitButton")
        sheet.addChildWindow(btn)
        btn.setText("Quit!")
        btn.setPosition(CEGUI.UVector2(cegui_reldim(0.035), cegui_reldim( 0.0)))
        btn.setSize(CEGUI.UVector2(cegui_reldim(0.1), cegui_reldim( 0.036)))
        
        btn.subscribeEvent(CEGUI.PushButton.EventClicked, self, "handleQuit")
        btn.setAlwaysOnTop(True)
        
        ## Alpha-slider
        slider = winMgr.createWindow("TaharezLook/Slider", "my slider")
        sheet.addChildWindow(slider)
        slider.setPosition(CEGUI.UVector2(cegui_reldim(0.136), cegui_reldim( 0.0)))
        slider.setSize(CEGUI.UVector2(cegui_reldim(0.02), cegui_reldim( 0.075)))
        slider.setAlwaysOnTop(True)
        ## here we disable inherited alpha so that we will always be able to see the slider
        slider.setInheritsAlpha(False)
        ## set up slider config
        slider.setCurrentValue(1.0)
        slider.setClickStep(0.1)
        ## subscribe handler that will process slider position changes.
        slider.subscribeEvent(CEGUI.Slider.EventValueChanged, sliderHandler, "")
        
        ## big scroll bar
        vscb = winMgr.createWindow("TaharezLook/LargeVerticalScrollbar", "Vert Scroll 1")
        sheet.addChildWindow(vscb)
        vscb.setPosition(CEGUI.UVector2(cegui_reldim(0.0), cegui_reldim( 0.0)))
        vscb.setMinSize(CEGUI.UVector2(cegui_reldim(0.01), cegui_reldim( 0.01)))
        vscb.setMaxSize(CEGUI.UVector2(cegui_reldim(1.0), cegui_reldim( 1.0)))
        vscb.setSize(CEGUI.UVector2(cegui_reldim(0.035), cegui_reldim( 0.83)))
        vscb.setDocumentSize(100)
        vscb.setPageSize(5)
        vscb.setStepSize(1)
        vscb.setAlwaysOnTop(True)
        
        ## mini vert scroll bar
        mvsb = winMgr.createWindow("TaharezLook/VerticalScrollbar", "MiniVertScroll 1")
        sheet.addChildWindow(mvsb)
        mvsb.setPosition(CEGUI.UVector2(cegui_reldim(0.99), cegui_reldim( 0.015)))
        mvsb.setMinSize(CEGUI.UVector2(cegui_reldim(0.01), cegui_reldim( 0.01)))
        mvsb.setMaxSize(CEGUI.UVector2(cegui_reldim(1.0), cegui_reldim( 1.0)))
        mvsb.setSize(CEGUI.UVector2(cegui_reldim(0.01), cegui_reldim( 0.5)))
        mvsb.setDocumentSize(360)
        mvsb.setPageSize(45)
        mvsb.setStepSize(1)
        mvsb.setAlwaysOnTop(True)
        
        ## mini horz scroll bar
        mhsb = winMgr.createWindow("TaharezLook/HorizontalScrollbar", "MiniHorzScroll 1")
        sheet.addChildWindow(mhsb)
        mhsb.setPosition(CEGUI.UVector2(cegui_reldim(0.485), cegui_reldim( 0.0)))
        mhsb.setMinSize(CEGUI.UVector2(cegui_reldim(0.01), cegui_reldim( 0.01)))
        mhsb.setMaxSize(CEGUI.UVector2(cegui_reldim(1.0), cegui_reldim( 1.0)))
        mhsb.setSize(CEGUI.UVector2(cegui_reldim(0.5), cegui_reldim( 0.015)))
        mhsb.setDocumentSize(360)
        mhsb.setPageSize(45)
        mhsb.setStepSize(1)
        mhsb.setAlwaysOnTop(True)
            
        ##
        ## Build a window with some text and formatting options via radio buttons etc
        ##
        textwnd = winMgr.createWindow("TaharezLook/FrameWindow", "TextWindow")
        sheet.addChildWindow(textwnd)
        textwnd.setPosition(CEGUI.UVector2(cegui_reldim(0.2), cegui_reldim( 0.2)))
        textwnd.setMaxSize(CEGUI.UVector2(cegui_reldim(0.75), cegui_reldim( 0.75)))
        textwnd.setMinSize(CEGUI.UVector2(cegui_reldim(0.1), cegui_reldim( 0.1)))
        textwnd.setSize(CEGUI.UVector2(cegui_reldim(0.5), cegui_reldim( 0.5)))
        textwnd.setCloseButtonEnabled(False)
        textwnd.setText("Crazy Eddie's GUI - Demo 4")
        
        st = winMgr.createWindow("TaharezLook/StaticText", "TextWindow/Static")
        textwnd.addChildWindow(st)
        st.setPosition(CEGUI.UVector2(cegui_reldim(0.1), cegui_reldim( 0.2)))
        st.setSize(CEGUI.UVector2(cegui_reldim(0.5), cegui_reldim( 0.6)))
        
        st = winMgr.createWindow("TaharezLook/StaticText", "TextWindow/Group label 1")
        textwnd.addChildWindow(st)
        st.setPosition(CEGUI.UVector2(cegui_reldim(0.65), cegui_reldim( 0.23)))
        st.setSize(CEGUI.UVector2(cegui_reldim(0.35), cegui_reldim( 0.05)))
        st.setText("Horz. Formatting")
        ## disable frame and background on static control
        st.setProperty("FrameEnabled", "False")
        st.setProperty("BackgroundEnabled", "False")
        
        st = winMgr.createWindow("TaharezLook/StaticText", "TextWindow/Group label 2")
        textwnd.addChildWindow(st)
        st.setPosition(CEGUI.UVector2(cegui_reldim(0.65), cegui_reldim( 0.53)))
        st.setSize(CEGUI.UVector2(cegui_reldim(0.35), cegui_reldim( 0.05)))
        st.setText("Vert. Formatting")
        ## disable frame and background on static control
        st.setProperty("FrameEnabled", "False")
        st.setProperty("BackgroundEnabled", "False")
        
        st = winMgr.createWindow("TaharezLook/StaticText", "TextWindow/Box label")
        textwnd.addChildWindow(st)
        st.setPosition(CEGUI.UVector2(cegui_reldim(0.12), cegui_reldim( 0.13)))
        st.setSize(CEGUI.UVector2(cegui_reldim(0.35), cegui_reldim( 0.05)))
        st.setText("Formatted Output")
        ## disable frame and background on static control
        st.setProperty("FrameEnabled", "False")
        st.setProperty("BackgroundEnabled", "False")
        
        ## word-wrap checkbox
        cb = winMgr.createWindow("TaharezLook/Checkbox", "TextWindow/CB1")
        textwnd.addChildWindow(cb)
        cb.setPosition(CEGUI.UVector2(cegui_reldim(0.65), cegui_reldim( 0.13)))
        cb.setSize(CEGUI.UVector2(cegui_reldim(0.35), cegui_reldim( 0.05)))
        cb.setText("Word Wrap")
        ## subscribe a handler to listen for when the check-box button select state changes
        cb.subscribeEvent(CEGUI.Checkbox.EventCheckStateChanged, formatChangedHandler, "")
        
        ## horizontal formatting radio group
        rb = winMgr.createWindow("TaharezLook/RadioButton", "TextWindow/RB1")
        textwnd.addChildWindow(rb)
        rb.setPosition(CEGUI.UVector2(cegui_reldim(0.65), cegui_reldim( 0.3)))
        rb.setSize(CEGUI.UVector2(cegui_reldim(0.35), cegui_reldim( 0.05)))
        rb.setGroupID(1)
        rb.setText("Left Aligned")
        ## subscribe a handler to listen for when the radio button select state changes
        rb.subscribeEvent(CEGUI.RadioButton.EventSelectStateChanged, formatChangedHandler, "")
        
        rb = winMgr.createWindow("TaharezLook/RadioButton", "TextWindow/RB2")
        textwnd.addChildWindow(rb)
        rb.setPosition(CEGUI.UVector2(cegui_reldim(0.65), cegui_reldim( 0.35)))
        rb.setSize(CEGUI.UVector2(cegui_reldim(0.35), cegui_reldim( 0.05)))
        rb.setGroupID(1)
        rb.setText("Right Aligned")
        ## subscribe a handler to listen for when the radio button select state changes
        rb.subscribeEvent(CEGUI.RadioButton.EventSelectStateChanged, formatChangedHandler,"")
        
        rb = winMgr.createWindow("TaharezLook/RadioButton", "TextWindow/RB3")
        textwnd.addChildWindow(rb)
        rb.setPosition(CEGUI.UVector2(cegui_reldim(0.65), cegui_reldim( 0.4)))
        rb.setSize(CEGUI.UVector2(cegui_reldim(0.35), cegui_reldim( 0.05)))
        rb.setGroupID(1)
        rb.setText("Centred")
        ## subscribe a handler to listen for when the radio button select state changes
        rb.subscribeEvent(CEGUI.RadioButton.EventSelectStateChanged, formatChangedHandler, "")
               
        ## vertical formatting radio group
        rb = winMgr.createWindow("TaharezLook/RadioButton", "TextWindow/RB4")
        textwnd.addChildWindow(rb)
        rb.setPosition(CEGUI.UVector2(cegui_reldim(0.65), cegui_reldim( 0.6)))
        rb.setSize(CEGUI.UVector2(cegui_reldim(0.35), cegui_reldim( 0.05)))
        rb.setGroupID(2)
        rb.setText("Top Aligned")
        ## subscribe a handler to listen for when the radio button select state changes
        rb.subscribeEvent(CEGUI.RadioButton.EventSelectStateChanged, formatChangedHandler,"")
        
        rb = winMgr.createWindow("TaharezLook/RadioButton", "TextWindow/RB5")
        textwnd.addChildWindow(rb)
        rb.setPosition(CEGUI.UVector2(cegui_reldim(0.65), cegui_reldim( 0.65)))
        rb.setSize(CEGUI.UVector2(cegui_reldim(0.35), cegui_reldim( 0.05)))
        rb.setGroupID(2)
        rb.setText("Bottom Aligned")
        ## subscribe a handler to listen for when the radio button select state changes
        rb.subscribeEvent(CEGUI.RadioButton.EventSelectStateChanged, formatChangedHandler, "")
        
        rb = winMgr.createWindow("TaharezLook/RadioButton", "TextWindow/RB6")
        textwnd.addChildWindow(rb)
        rb.setPosition(CEGUI.UVector2(cegui_reldim(0.65), cegui_reldim( 0.7)))
        rb.setSize(CEGUI.UVector2(cegui_reldim(0.35), cegui_reldim( 0.05)))
        rb.setGroupID(2)
        rb.setText("Centred")
        ## subscribe a handler to listen for when the radio button select state changes
        rb.subscribeEvent(CEGUI.RadioButton.EventSelectStateChanged, formatChangedHandler, "")
        
        ## Edit box for text entry
        eb = winMgr.createWindow("TaharezLook/Editbox", "TextWindow/Editbox1")
        textwnd.addChildWindow(eb)
        eb.setPosition(CEGUI.UVector2(cegui_reldim(0.05), cegui_reldim( 0.85)))
        eb.setMaxSize(CEGUI.UVector2(cegui_reldim(1.0), cegui_reldim( 0.04)))
        eb.setSize(CEGUI.UVector2(cegui_reldim(0.90), cegui_reldim( 0.08)))
        ## subscribe a handler to listen for when the text changes
        eb.subscribeEvent(CEGUI.Window.EventTextChanged, textChangedHandler,"")
        
        ##
        ## Controls are set up.  Install initial settings
        ##
        winMgr.getWindow("TextWindow/CB1").setSelected(True)
        winMgr.getWindow("TextWindow/RB1").setSelected(True)
        winMgr.getWindow("TextWindow/RB4").setSelected(True)
        winMgr.getWindow("TextWindow/Editbox1").setText("Come on then, edit me!")
        
        ## success!
        return True
    
    
    
    
        
                
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
# 				CEGUI.Combobox.EventListSelectionAccepted, self, "handleObjectSelection")

				                
    def handleQuit(self, e):
        self.frameListener.requestShutdown() 
        return True

# /*************************************************************************
#     Free function handler called when editbox text changes
# *************************************************************************/
def textChangedHandler( e):

    ##find the static box
    st = CEGUI.WindowManager.getSingleton().getWindow("TextWindow/Static")

    ## set text from the edit box...
    st.setText(e.window.getText())

    return True
        
        
        
# /*************************************************************************
#     Free function to handle slider position changes
# *************************************************************************/
def sliderHandler( e):
    ## we know it's a slider.
    s = e.window

    ## get value from slider and set it as the current alpha
    val = s.getCurrentValue()
    CEGUI.WindowManager.getSingleton().getWindow("root_wnd").setAlpha(val)

    ## indicate the event was handled here.
    return True

# /*************************************************************************
#     Free function to handle change of format options
# *************************************************************************/
def formatChangedHandler( e):
    
    ## we will use the WindowManager to get access to the widgets
    winMgr = CEGUI.WindowManager.getSingleton()
    
    ## get pointers to all the widgets we need to access
    rb1 = winMgr.getWindow("TextWindow/RB1")
    rb2 = winMgr.getWindow("TextWindow/RB2")
    rb3 = winMgr.getWindow("TextWindow/RB3")
    rb4 = winMgr.getWindow("TextWindow/RB4")
    rb5 = winMgr.getWindow("TextWindow/RB5")
    rb6 = winMgr.getWindow("TextWindow/RB6")
    cb1 = winMgr.getWindow("TextWindow/CB1")
    ## and also the static text for which we will set the formatting options
    st  = winMgr.getWindow("TextWindow/Static")
    
    ## handle vertical formatting settings
    if rb4.isSelected():
        st.setProperty("VertFormatting", "TopAligned")
    elif rb5.isSelected():
        st.setProperty("VertFormatting", "BottomAligned")
    elif rb6.isSelected():
        st.setProperty("VertFormatting", "VertCentred")
    
    ## handle horizontal formatting settings
    wrap = cb1.isSelected()
    
    if rb1.isSelected():
        if wrap:
            st.setProperty("HorzFormatting","WordWrapLeftAligned")
        else:
            st.setProperty("HorzFormatting", "LeftAligned")
    elif rb2.isSelected():
        if wrap:
            st.setProperty("HorzFormatting",  "WordWrapRightAligned" )
        else:
            st.setProperty("HorzFormatting", "RightAligned")
    elif rb3.isSelected():
        if wrap:
            st.setProperty("HorzFormatting",  "WordWrapCentred" )
        else:
            st.setProperty("HorzFormatting",  "HorzCentred")
    
    ## event was handled
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
    
        

