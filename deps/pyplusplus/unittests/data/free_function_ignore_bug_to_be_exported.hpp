// Copyright 2004 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __free_function_ignore_bug_to_be_exported_hpp__
#define __free_function_ignore_bug_to_be_exported_hpp__

#include <string>

namespace free_function_ignore_bug{ 

inline void do_nothing(){}
    
inline void do_nothing(int, int){}

}

#endif//__user_text_to_be_exported_hpp__
