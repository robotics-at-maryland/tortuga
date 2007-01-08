#ifndef __python_ois_h_11
#define __python_ois_h_11

//See best practices section in Py++ documentation

#include <vector>
#include <string>
#include <map>
#include "OIS.h"

namespace python_ois{ 

inline void instantiate(){
    using namespace OIS;
    OIS::ParamList t;
    sizeof ( t );
    sizeof (ParamList);
    sizeof ( Mouse * );
    sizeof ( Keyboard *  );
    sizeof ( JoyStick * );
    sizeof ( Mouse  );
    sizeof ( Keyboard );
    sizeof ( JoyStick );
    
 
    typedef std::map<std::string, std::string> ParamList1;
    sizeof ( ParamList1);
    
    // Named Vale Pair list
    sizeof ( std::map<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > > );

    // force paramlist
    sizeof ( std::map<std::string, std::string>);
    sizeof ( std::multimap<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char,std::char_traits<char>, std::allocator<char> >, std::less<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator< std::pair< std::basic_string<char, std::char_traits<char>, std::allocator<char> > const , std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > >);
    sizeof ( class std::multimap<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::less<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > >,class std::allocator<struct std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > > > > );
    sizeof ( std::multimap<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string, std::string> > >);
//     sizeof ( std::multimap<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > > [class]

//    sizeof( Controller<float> );
    // this is needed to expose within DataStream.pypp.cpp - required as a return value 
    // from with in resource group manager
//    sizeof( SharedPtr<DataStream> );
    
    // lets expose the other shared pointers just in case :)
/*
    sizeof( SharedPtr<AnimableValuePtr> );
    sizeof( SharedPtr<ControllerValueRealPtr> );
    sizeof( SharedPtr<ControllerFunctionRealPtr> );
    sizeof( SharedPtr<DataStreamPtr> );
    sizeof( SharedPtr<ResourcePtr> );
    sizeof( SharedPtr<GpuProgramParametersSharedPtr> );
    */
    
//     sizeof ( std::map<String, String> );    // NameValuePairList
//   
//             
//     //sizeof(class std::map<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::less<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > >,class std::allocator<struct std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > > > >);
//     
//     // needed for configfile section iterator..
//     sizeof ( MapIterator<std::map<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::multimap<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::less<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > >*, std::less<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::multimap<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::less<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > >*> > > > );
//     // needed for configfile settings iterator..
//     sizeof ( MapIterator<std::multimap<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::less<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > > >);
//     // SettingsBySection
//     sizeof ( std::multimap<std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::less<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >,std::allocator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > >);
//     // Section Iterator - getnext..
//     sizeof ( MapIterator<std::map<std::string, std::multimap<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > >*, std::less<std::string>, std::allocator<std::pair<std::string const, std::multimap<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > >*> > > > );
    
 
}

} //python_ogre::details


#endif//__python_ogre_h_5B402FBF_8DE8_49C9_AF71_1CF9634B5344__
