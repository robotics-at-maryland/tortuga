// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __optional_to_be_exported_hpp__
#define __optional_to_be_exported_hpp__

namespace optional_args{
    
struct data
{
    data(int i=0, int j=1) 
    : m_i( i ), m_j( j )
    {}
        
    int m_i;
    int m_j;

};

struct data2
{
    data2(int i, int j=1) 
    : m_i( i ), m_j( j )
    {}
        
    int m_i;
    int m_j;

};

}//optional_args 

#endif//__optional_to_be_exported_hpp__

