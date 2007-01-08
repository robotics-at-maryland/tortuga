def aliases( Version ):
    if Version == "0.5.0b" :
        return {
#           "DataContainer<unsigned char>"
#           :  "DataContainerUnsignedChar"
#           ,"RefPtr<CEGUI::Event::ConnectionInterface>"
#           : "RefPtrConnectionInterface"
         "Singleton<CEGUI::Logger>"
              : "SingletonLogger"
         ,"Singleton<CEGUI::FontManager>"
               : "SingletonFontManager"
         ,"Singleton<CEGUI::GlobalEventSet>"
               : "SingletonGlobalEventSet"
         ,"Singleton<CEGUI::ImagesetManager>"
               : "SingletonImagesetManager"
         ,"Singleton<CEGUI::MouseCursor>"
               : "SingletonMouseCursor"
         ,"Singleton<CEGUI::SchemeManager>"
               : "SingletonSchemeManager"
         ,"Singleton<CEGUI::System>"
               : "SingletonSystem"
         ,"Singleton<CEGUI::WidgetLookManager>"
               : "SingletonWidgetLookManager"
         ,"Singleton<CEGUI::WindowFactoryManager>"
               : "SingletonWindowFactoryManager"
         ,"Singleton<CEGUI::WindowManager>"
               : "SingletonWindowManager"
         ,"Singleton<CEGUI::WindowRendererManager>"
               : "SingletonWindowRendererManager"
         ,"RefCounted<CEGUI::BoundSlot>"
               : "RefCountedBoundSlot"
        }

        
def header_files( Version ):
    return [   "CEGUI.h"
            , "CEGUIXMLParser.h"
            , "OgreCEGUIRenderer.h"
#             , "OgreRenderQueue.h"
            , "CEGUIXMLHandler.h"
            , "CEGUIXMLAttributes.h"
            , "CEGUIXMLserializer.h"
            , "OgreCEGUIRenderer.h"
            ,"OgreCEGUIResourceProvider.h"
                  ]
#       return [ "CEGUI.h"
#             , "CEGUIXMLParser.h"
#             , "OgreCEGUIRenderer.h"
#             , "OgreRenderQueue.h"
#             , "CEGUIXMLHandler.h"
#             , "CEGUIXMLAttributes.h"
#             , "CEGUIXMLserializer.h"
#             , "OgreCEGUIRenderer.h"
#               ]
#     return [ 'CEGUI/CEGUIImageset.h'
#     ,'CEGUI/CEGUISystem.h'
#     ,'CEGUI/CEGUILogger.h'
#     ,'CEGUI/CEGUISchemeManager.h'
#     ,'CEGUI/CEGUIWindowManager.h'
#     ,'CEGUI/CEGUIWindow.h'
#     ,'CEGUI/CEGUIPropertyHelper.h'
#     ,'CEGUI/elements/CEGUICombobox.h'
#     ,'CEGUI/elements/CEGUIListbox.h'
#     ,'CEGUI/elements/CEGUIListboxTextItem.h'
#     ,'CEGUI/elements/CEGUIPushButton.h'
#     ,'CEGUI/elements/CEGUIScrollbar.h'
#     ,"OgreCEGUIRenderer.h"
#     ,"OgreCEGUIResourceProvider.h"
#     ]

def huge_classes( Version ):
    return []
