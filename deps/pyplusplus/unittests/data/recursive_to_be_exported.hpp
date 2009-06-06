// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __recursive__to_be_exported_hpp__
#define __recursive__to_be_exported_hpp__

namespace recursive{

struct fields_t{
    fields_t()
    : skip_a(112)
    {}
    
    const unsigned int skip_a : 11;    
    unsigned int : 0;
    unsigned int skip_b : 1;        
    
};

}

#endif//__recursive__to_be_exported_hpp__