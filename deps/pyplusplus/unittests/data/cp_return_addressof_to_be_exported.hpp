// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __cp_return_addressof_to_be_exported_hpp__
#define __cp_return_addressof_to_be_exported_hpp__

#include <string>

struct image_t{
    image_t()
    : buffer( new int[5] )
    {
        for( int i = 0; i < 5; ++i ){
            buffer[i] = i;
        }
    }

    ~image_t(){ delete[] buffer; }

    int* get_buffer() const { return buffer; }

private:
    int* buffer;
};

#endif//__cp_return_addressof_to_be_exported_hpp__
