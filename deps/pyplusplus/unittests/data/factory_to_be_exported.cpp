// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "factory_to_be_exported.hpp"

namespace factory{

class concrete : public abstract{
    virtual int run( ) const{
        return 1;
    }
};

 
std::auto_ptr<abstract> create(){
    return std::auto_ptr<abstract>( new concrete() );
}

    
}