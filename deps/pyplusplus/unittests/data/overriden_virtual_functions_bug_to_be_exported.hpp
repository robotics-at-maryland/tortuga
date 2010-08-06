// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __overriden_virtual_functions_bug_to_be_exported_hpp__
#define __overriden_virtual_functions_bug_to_be_exported_hpp__

namespace vector3{

struct window_t{
protected:
    virtual void on_mouse_leaves( int ){};
};

struct button_based_t : public window_t{
protected:    
    virtual void on_mouse_leaves( int ){};    
};

struct checkbox_t : public button_based_t{
};

}

#endif//__overriden_virtual_functions_bug_to_be_exported_hpp__
