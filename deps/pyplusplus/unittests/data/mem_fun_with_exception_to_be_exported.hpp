// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __mem_fun_with_exception_to_be_exported_hpp__
#define __mem_fun_with_exception_to_be_exported_hpp__

#include <stdexcept>

namespace mem_fun_with_exception{ 

struct data_t{
public:
    virtual void do_nothing() throw( std::exception )
    {}
};

}

#endif//__mem_fun_with_exception_to_be_exported_hpp__


