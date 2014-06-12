// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __deepcopy_to_be_exported_hpp__
#define __deepcopy_to_be_exported_hpp__

#include <vector>

namespace deepcopy{

struct item_t{
    item_t(){
        id = reinterpret_cast< int >( this );
    }
    int id;
};

typedef std::vector< item_t > items_t;

inline items_t create_items(){
    items_t items;
    items.push_back( item_t() );
    items.push_back( item_t() );
    items.push_back( item_t() );
    return items;
}

}//statics


#endif//__deepcopy_to_be_exported_hpp__
