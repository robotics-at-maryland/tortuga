#ifndef __python_CEGUI_h_01
#define __python_CEGUI_h_01

//See best practices section in Py++ documentation

#include <vector>
#include <string>
#include <map>
#include <iostream>


// #include <CEGUI/CEGUIImageset.h>
// #include <CEGUI/CEGUISystem.h>
// #include <CEGUI/CEGUILogger.h>
// #include <CEGUI/CEGUISchemeManager.h>
// #include <CEGUI/CEGUIWindowManager.h>
// #include <CEGUI/CEGUIWindow.h>
// #include <CEGUI/CEGUIPropertyHelper.h>
// #include <CEGUI/elements/CEGUICombobox.h>
// #include <CEGUI/elements/CEGUIListbox.h>
// #include <CEGUI/elements/CEGUIListboxTextItem.h>
// #include <CEGUI/elements/CEGUIPushButton.h>
// #include <CEGUI/elements/CEGUIScrollbar.h>

#include "CEGUI.h"
#include "CEGUIXMLParser.h"     // needed for System
#include "CEGUIXMLHandler.h"
#include "CEGUIXMLAttributes.h" // needed for FontManager
#include "CEGUIXMLserializer.h"
#include "OgreCEGUIRenderer.h"
#include "OgreCEGUIResourceProvider.h"

// namespace  CEGUI {

// class EventConnection
// {
// public:
//     EventConnection();
//     bool connected();
//     void disconnect();
// };
// }
namespace python_CEGUI{ 

    inline void instantiate(){
        using namespace CEGUI;
        sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::Imageset*, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::Imageset*> > > > );
        sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::Property*, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::Property*> > > > );
        sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::Scheme*, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::Scheme*> > > > );
        sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::Window*, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::Window*> > > > );
        sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::WindowFactoryManager::FalagardWindowMapping, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::WindowFactoryManager::FalagardWindowMapping> > > > );
        sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::Event*, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::Event*> > > > );
        sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::WindowFactoryManager::AliasTargetStack, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::WindowFactoryManager::AliasTargetStack> > > > );
        sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::WindowFactory*, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::WindowFactory*> > > > );
        sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::Font*, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::Font*> > > > );
        sizeof(  CEGUI::ConstBaseIterator<std::map<CEGUI::String, CEGUI::Image, CEGUI::String::FastLessCompare, std::allocator<std::pair<const CEGUI::String, CEGUI::Image> > > > );


    }

} 

#endif
