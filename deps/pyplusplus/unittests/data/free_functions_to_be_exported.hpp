// Copyright 2004 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __free_functions_to_be_exported_hpp__
#define __free_functions_to_be_exported_hpp__

namespace free_functions{

inline int one(){ 
    return 1; 
}

inline int plus(int a, const int b=21){ 
    return a+b; 
}

}

#endif//__free_functions_to_be_exported_hpp__

