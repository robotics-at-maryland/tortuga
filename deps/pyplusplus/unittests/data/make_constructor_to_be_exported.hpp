// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __make_constructor_to_be_exported_hpp__
#define __make_constructor_to_be_exported_hpp__

#include <memory>

namespace mc{

struct numbers_t{
    numbers_t()
    : x( 0 )
      , y( 0 )
    {}
    ~numbers_t(){}

    static std::auto_ptr<numbers_t> create( int i, int j, int k, int m ){
        std::auto_ptr<numbers_t> n( new numbers_t() );
        n->x = i + j;
        n->y = k + m;
        return n;
    }

    int x;
    int y;
};

inline std::auto_ptr<numbers_t> create( int i, int j){
    std::auto_ptr<numbers_t> n( new numbers_t() );
    n->x = i;
    n->y = j;
    return n;
}

inline std::auto_ptr<numbers_t> create( double, double, double, double, double){
    return std::auto_ptr<numbers_t>();
}


}

#endif//__make_constructor_to_be_exported_hpp__
