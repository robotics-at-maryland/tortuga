// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __global_variables_to_be_exported_hpp__
#define __global_variables_to_be_exported_hpp__

namespace global_variables{

enum color{ red, green, blue };

extern const color const_var;
extern color non_const_var;

struct data{ int value; };
extern data garray[10];

extern double arr_of_doubles[100];
void init_garray();

extern int some_value;

int get_some_value();

unsigned int get_some_value_address();

}


extern const char someSin[3];

#endif//__global_variables_to_be_exported_hpp__
