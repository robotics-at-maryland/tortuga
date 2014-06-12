// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __final_classes_to_be_exported_hpp__
#define __final_classes_to_be_exported_hpp__

#include <string>

namespace final_classes{

class Foo{
public:
    virtual void bar() {}
private:
    Foo() {}
};


} 

#endif//__final_classes_to_be_exported_hpp__

