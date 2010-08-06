// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __enums_to_be_exported_hpp__
#define __enums_to_be_exported_hpp__

#include "boost/rational.hpp"

class C{
public:
    
    template<typename T>
    T get_value() const{
        return static_cast<T>(m_value);
    }
 
private:
    int m_value;
};

struct instantiator_t{
    instantiator_t(){
        boost::gcd<long int>( 1, 1);
        C c;
        c.get_value<int>();
    }
};

#endif//__enums_to_be_exported_hpp__
