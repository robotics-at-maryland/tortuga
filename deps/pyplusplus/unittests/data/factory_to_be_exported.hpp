// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __factory_to_be_exported_hpp__
#define __factory_to_be_exported_hpp__

#include <memory>
    
namespace factory{
 
class abstract{
public:
    virtual int run() const = 0;
};

std::auto_ptr<abstract> create();
    
}

#endif//__factory_to_be_exported_hpp__
