// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __special_operators_to_be_exported_hpp__
#define __special_operators_to_be_exported_hpp__

#include <string>

namespace special_operators{ 

struct operators_t{
    
    operator int() const { return 28; }
    
    operator double() { return 3.5; }
    
    operator const char* () const { return "hello world"; }
    
    int operator()(){ return 11; }
};

struct to_string_t{
    operator std::string() const { return "hello world"; }
};

struct to_wstring_t{
    operator std::wstring() const { return L"hello world"; }
};

struct call_t{
    int operator()( int x, int y ){ return x + y; }
    double operator()( double x){ return x; }
    
    virtual bool operator()( bool x){ return x; }
};

struct base_call_t{
    virtual int operator()( int x, int y ) const = 0;
};

struct derive_call_t : base_call_t{
    virtual int operator()( int x, int y ) const { return x + y; }
};

int virtual_call( base_call_t& x, int arg1, int arg2 ){ 
    return x( arg1, arg2 );
}

}

#endif//__special_operators_to_be_exported_hpp__
