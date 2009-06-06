// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __inner_class_bug_to_be_exported_hpp__
#define __inner_class_bug_to_be_exported_hpp__

namespace inner_classes {

struct main_t{

    void main_method(){}
    virtual void main_method2(){}
    virtual void main_method3() = 0;

    struct inner_t{
        inner_t( int ){}
        void inner_method(){}
        virtual void inner_method2(){}
        virtual void inner_method3() = 0;
    };
};
    
}

#endif//__inner_class_bug_to_be_exported_hpp__
