// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __custom_string_to_be_exported_hpp__
#define __custom_string_to_be_exported_hpp__

#include <string>

namespace custom_strings{

class utf16_t{
public:
    utf16_t() {}
    explicit utf16_t(std::string const& value) : m_value_a(value) {}
    explicit utf16_t(std::wstring const& value) : m_value_w(value) {}   
    
    utf16_t( const utf16_t& other )
    : m_value_a( other.m_value_a ), m_value_w( other.m_value_w )
    {}
      
    utf16_t& operator=( const utf16_t& other ){
        m_value_a = other.m_value_a;
        m_value_w = other.m_value_w;
        return *this;
    }
    
    std::string const& value_a() const { return m_value_a; }
    std::wstring const& value_w() const { return m_value_w; }

private:    
    std::wstring m_value_w;
    std::string m_value_a;
};

struct name_t{
    name_t() : m_123( "123" ){}
    
    const utf16_t& get_name() const { return m_name; }
    void set_name(const utf16_t& n) { m_name = n; }
        
    utf16_t m_name;
    const utf16_t m_123;
};

inline std::string utf16_to_string( const utf16_t& x ){
    return x.value_a();
}

inline std::wstring utf16_to_wstring( const utf16_t& x ){
    return x.value_w();
}

}

#endif//__custom_string_to_be_exported_hpp__
