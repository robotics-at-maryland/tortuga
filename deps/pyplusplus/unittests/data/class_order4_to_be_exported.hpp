// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __class_order4_to_be_exported_hpp__
#define __class_order4_to_be_exported_hpp__

namespace class_order4{ 
    
struct container{
    enum fruits{ apple, orange };
    
    container()
    : my_fruit(orange)
    {}

    explicit container( int, fruits x = apple )
    : my_fruit( x )
    {}
    
    fruits my_fruit;        
};    

}

#endif//__class_order4_to_be_exported_hpp__

