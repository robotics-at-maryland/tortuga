// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __precompiled_header_to_be_exported_hpp__
#define __precompiled_header_to_be_exported_hpp__

namespace enums{ 

enum color{ red, green, blue}; 
    
inline int to_int( int x=red ){ return x; }

}


#endif//__precompiled_header_to_be_exported_hpp__
