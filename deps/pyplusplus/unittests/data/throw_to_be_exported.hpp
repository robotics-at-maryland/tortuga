// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __throw_to_be_exported_hpp__
#define __throw_to_be_exported_hpp__

namespace throw_exceptions{ 

struct my_exception{
};

inline void raise_nothing() throw() {};
inline void raise_nothing(int) throw() {};
inline void raise_always() throw( my_exception ){};
inline void raise_always(int) throw( my_exception ){};

struct mem_fun_throw_exception_t{
    void raise_nothing() throw() {};
    virtual void raise_nothing(int) throw() {};
    virtual void raise_always() throw( my_exception ){};
    virtual void raise_always(int) throw( my_exception ){};
};

    
}

#endif//__throw_to_be_exported_hpp__
