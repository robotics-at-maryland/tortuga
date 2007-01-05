// Copyright 2004 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __operators_to_be_exported_hpp__
#define __operators_to_be_exported_hpp__

#include "boost/rational.hpp"
#include <iostream>

namespace pyplusplus{ namespace rational{

typedef boost::rational< long int > pyrational;

struct helper{
    
    void instantiate(){
        sizeof( pyrational );
        boost::gcd<long int>( 1, 1);
        boost::lcm<long int>( 1, 1);
        std::cout << pyrational( 1,1);        
    }
};

} }
    

#endif//__operators_to_be_exported_hpp__
