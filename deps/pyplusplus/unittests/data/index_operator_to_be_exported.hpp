// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __index_operator_to_be_exported_hpp__
#define __index_operator_to_be_exported_hpp__

namespace operators{

struct container{
    
    int operator[]( int index ){    
        return index % 2;
    }
    
};
}
    

#endif//__index_operator_to_be_exported_hpp__
