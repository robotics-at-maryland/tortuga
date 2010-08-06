// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __private_assign_to_be_exported_hpp__
#define __private_assign_to_be_exported_hpp__
#include <memory>
#include "boost/shared_ptr.hpp"

namespace private_assign{

class item_t{
public:
    item_t() : m_value ( 24 ) {}

private:
    item_t& operator=( const item_t& other ){
        m_value = other.m_value;
    }

public:
    int m_value;
};

struct container_t{
    container_t(){
        m_item.m_value = 23;
    }
    item_t m_item;
};

}

#endif//__smart_pointers_to_be_exported_hpp__
