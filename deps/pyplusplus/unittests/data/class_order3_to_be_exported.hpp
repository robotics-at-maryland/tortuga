// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __class_order3_to_be_exported_hpp__
#define __class_order3_to_be_exported_hpp__

namespace class_order3{ 

struct consts{
    enum fruits{ apple, orange };
};

struct container{
    explicit container( consts::fruits x = consts::apple )
    : my_fruit( x )
    {}
    
    consts::fruits my_fruit;        
};    

}

#endif//__class_order3_to_be_exported_hpp__

