// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __inner_tmpl_class_to_be_exported_hpp__
#define __inner_tmpl_class_to_be_exported_hpp__

#include <string>

namespace inner_tmpl_class{
    
struct Outer {
    template <typename T>
    struct Inner{
        Inner(T t_) :  t( t_ )
        {}
            
        T t;
    };
 
    typedef Inner<int> Inner_int;
    Inner_int f() { return Inner_int(23); }
};

} 

#endif//__inner_tmpl_class_to_be_exported_hpp__
