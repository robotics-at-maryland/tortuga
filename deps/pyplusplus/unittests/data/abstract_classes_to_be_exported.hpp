// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __abstract_classes_to_be_exported_hpp__
#define __abstract_classes_to_be_exported_hpp__

namespace abstract_classes{
    
struct base{
    virtual int do_smth() = 0;
};

struct derived : public base{
    virtual int do_smth_else() = 0;
};


}//classes 

#endif//__classes_to_be_exported_hpp__

