// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __non_overridable_to_be_exported_hpp__
#define __non_overridable_to_be_exported_hpp__
#include <string>

namespace non_overridables{ 

struct non_overridable_v_t{

    
    static bool test( const non_overridable_v_t& impl ){
        return impl.void_ptr() ? true : false;
    }

    
    virtual const void* void_ptr() const {
        return this;
    }
    
    virtual std::string& string_ref() const {
        static std::string x( "string_ref" );
        return x;
    }        
    
};

struct non_overridable_pv_t{

    static bool test( const non_overridable_pv_t& impl ){
        return impl.void_ptr() ? true : false;
    }
    
    virtual void* void_ptr() const = 0;
    
    virtual std::string& string_ref() const = 0;
    
};



}

#endif//__non_overridable_to_be_exported_hpp__
