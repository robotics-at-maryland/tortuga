// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __default_args_to_be_exported_hpp__
#define __default_args_to_be_exported_hpp__

namespace default_args{

struct data{
    int sum( int i=0 ){ return i; }
    int sum( int a, int b, int c=1, int d=2 ){ return a + b + c + d; }
};

}

#endif//__default_args_to_be_exported_hpp__
