// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __class_order_to_be_exported_hpp__
#define __class_order_to_be_exported_hpp__

namespace class_order{ 

struct item{
    item( int i, int j )
    : m_value( i + j )
    {}
    
    int m_value;
};

struct container{
public:    

    explicit container( const item& x = item( 2, 1 ) )
    : my_item( x )
    {}

    item my_item;        
};    

}

#endif//__class_order_to_be_exported_hpp__