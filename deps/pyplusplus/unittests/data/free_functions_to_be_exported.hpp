// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __free_functions_to_be_exported_hpp__
#define __free_functions_to_be_exported_hpp__

#include <stddef.h>

namespace free_functions{

inline int one(){ 
    return 1; 
}

inline int plus(int a, const int b=21){ 
    return a+b; 
}

inline void do_smth( size_t , int ){
}

}

#endif//__free_functions_to_be_exported_hpp__

