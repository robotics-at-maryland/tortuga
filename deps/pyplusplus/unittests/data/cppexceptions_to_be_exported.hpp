// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __cppexceptions_to_be_exported_hpp__
#define __cppexceptions_to_be_exported_hpp__

#include <string>

namespace cppexceptions{ 

struct custom_exception_t{
    custom_exception_t( const std::string& error )
    : m_error( error )
    {}
        
    const std::string& what() const{
        return m_error;
    }

private:
    const std::string m_error;
};
    
inline void throw_custom_exception(){
    throw custom_exception_t( "profe of concept" );
}

}

#endif//__cppexceptions_to_be_exported_hpp__
