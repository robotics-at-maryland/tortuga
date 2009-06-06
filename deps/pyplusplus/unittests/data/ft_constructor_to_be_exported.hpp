// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __ft_constructor_to_be_exported_hpp__
#define __ft_constructor_to_be_exported_hpp__

#include "boost/noncopyable.hpp"

namespace ft_constructor{

struct resources_t{
    resources_t(){
        std::cout << "created";
    }
    ~resources_t(){
        std::cout << "destroyed";
    }
};

struct holder_t : public boost::noncopyable{
    holder_t(resources_t* r=0)
    : resource( r )
    {}
        
    ~holder_t(){ delete resource; }
    
    const resources_t* get_resource() const { return resource; }
    
private:
    resources_t* resource;
};

}

#endif//__ft_constructor_to_be_exported_hpp__
