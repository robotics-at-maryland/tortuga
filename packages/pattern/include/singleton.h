/*
 *  (c) Copyright 2006
 *	Space Systems Laboratory, University of Maryland, College Park, MD 20742
 */

#ifndef RAM_PATTERN_SINGLETON_H_05_30_2007
#define RAM_PATTERN_SINGLETON_H_05_30_2007

// STD Includes
#include <cassert>

namespace ram {
namespace pattern {

/* Original version Copyright (C) Scott Bilas, 2000.
* All rights reserved worldwide.
*
* This software is provided "as is" without express or implied
* warranties. You may freely copy and compile this source into
* applications you distribute provided that the copyright text
* below is included in the resulting source code, for example:
* "Portions Copyright (C) Scott Bilas, 2000"
*/
    
/* 
    I, Joseph Lisee, have made the following changes:
        * Added the documentation
        * Changed the style of the code (indentations, braket spaceing, etc) 
        * Removed the (void) function types
        * Added the text comments to the assertions.
*/
    
/*! This class ensures there is only one instance of your class in existance 
    @remarks
        There are some caveats, you must new, and the delete the class yourself
        but this allows you to control the the order of creation when you have
        multiple singletons.  These the allocation and clean up of the object
        must occur, after and before any getSingleton calls.
    @par
        To use this class you just inherite from a while specify your class as
        the template parameter for the class. You must also have the following 
        line at the top of you implementation file (ie. your cpp file):
        @code 
        template<> MyClass* Singleton<MyClass>::ms_Singleton = 0;
        @endcode
    @par
        If you class is placed inside a shared library and you would like to 
        use that class outside of the shared library you need to reimplement 
        the getSingleton methods.  Here is an example header file:
        @code
        #include "foundation/pattern/h/singleton.h"
    
        namespace ssl {
        
        class MyClass : public foundation::pattern::Singleton<MyClass>
        {
        public:
            MyClass();
            static MyManager& getSingleton(void);
            static MyManager* getSingletonPtr(void);
            // ... your method declarations here
        };
            
        } // namespace ssl
        @endcode
        Here is how the implementation file would look:
        @code
        #include "MyClass.h"
        
        namespace ssl {
            
        template<> MyClass* 
            foundation::pattern::Singleton<MyClass>::ms_Singleton = 0;
    
        MyClass& MyClass::getSingleton()
        {  
            assert(ms_Singleton && 
                   "Singleton instance or MyClass not created");  
            ms_Singleton = 0;    
        }
        MyClass* MyClass::getSingletonPtr()
        {
            return ms_Singleton;
        }  
    
        // ... the rest of the method definitions here
        } // namespace ssl
        @endcode
*/
template <typename T> class Singleton
{
protected:
    static T* ms_Singleton;
    
public:
    /*! Singleton constructor
        @warning
            This will assert if called more than once.
    */
    Singleton()
    {
        assert( !ms_Singleton && 
                "Cannot create more than one instance of a Singleton class");
    #if defined( _MSC_VER ) && _MSC_VER < 1200	 
        int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
        ms_Singleton = (T*)((int)this + offset);
    #else
        ms_Singleton = static_cast< T* >( this );
    #endif
    }
    
    /*! Sets the singleton instance pointer to 0, 
        @warning
            This will assert if the Singleton instance does not exist. This 
            would happen if you call delete more times than you call new on the
            class.
    */
    ~Singleton()
    {   
        assert(ms_Singleton && "Singleton instance not created");  
        ms_Singleton = 0;  
    }
    
    /*! Get a reference to the singleton instance, recommend way access method 
        @remarks
            This is gaurenteed to work or cause an assertion so you instantly
            know where the problem is.
        @warning
            This will assert if you have not new'ed the Singleton class.
    */
    static T& getSingleton()
    {
        assert(ms_Singleton && "Singleton instance not created");  
        return (*ms_Singleton); 
    }
    
    /*! A non-asserting way to access the Singleton instance
        @remakrs
            This lets you check for the existance.
    */
    static T* getSingletonPtr()
    { 
        return ms_Singleton; 
    }
};
    
} // namespace pattern
} // namespace ram
    
#endif // RAM_PATTERN_SINGLETON_H_05_30_2007
