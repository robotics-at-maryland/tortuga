// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __protected_to_be_exported_hpp__
#define __protected_to_be_exported_hpp__

namespace protected_{ 

struct protected_t{
protected:
    int get_1(){ return 1; }
};

struct protected_s_t{
protected:
    static int get_2(){ return 2; }
};
    

struct protected_v_t{
protected:
    virtual int get_i(){ return 10; }
};

struct protected_v_derived_t : public protected_v_t{
};

}

#endif//__protected_to_be_exported_hpp__


