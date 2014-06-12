// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __free_function_ignore_bug_to_be_exported_hpp__
#define __free_function_ignore_bug_to_be_exported_hpp__

#include <string>

namespace free_function_ignore_bug{ 

inline void do_nothing(){}
    
inline void do_nothing(int, int){}

struct some_dummy_struct_t{};

some_dummy_struct_t** return_some_dummy_struct_ptr_ptr(){
    return 0;
}

void fill_some_dummy_struct_ptr_ptr(some_dummy_struct_t**){}

}

#endif//__user_text_to_be_exported_hpp__
