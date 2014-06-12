// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __pointer_as_arg_to_be_exported_hpp__
#define __pointer_as_arg_to_be_exported_hpp__

#include <string>

namespace pointer_as_arg{ 

struct data{};
    
inline int
test_for_null( data* d=0 ){
    return d ? 1 : 0;
}    

}

#endif//__pointer_as_arg_to_be_exported_hpp__