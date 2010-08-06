// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __overloads_macro_to_be_exported_hpp__
#define __overloads_macro_to_be_exported_hpp__

namespace overloads_macro{

struct calculator_t{

    double add( int i, int j ){
        return i + j;
    }

    double add( int i, int j, int k ){
        return i * j + k;
    }

    double add( double i, double j ){
        return i + j;
    }


};

double add( int i, int j ){
    return i + j;
}

double add( int i, int j, int k ){
    return i * j + k;
}

double add( double i, double j ){
    return i + j;
}



}


#endif//__enums_to_be_exported_hpp__
