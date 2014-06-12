// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __rational_export_hpp__
#define __rational_export_hpp__

#include "boost/rational.hpp"

namespace boost{ namespace detail{

typedef boost::rational< long int > pyrational;

inline void instantiate(){
    sizeof( pyrational );
    boost::gcd<long int>( 1, 1);
    boost::lcm<long int>( 1, 1);
    boost::abs( pyrational(1) );
    boost::rational_cast<double>( pyrational(1) );
    boost::rational_cast<long int>( pyrational(1) );
}

} }


#endif//__rational_export_hpp__