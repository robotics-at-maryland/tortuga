// Copyright 2004 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __split_module_to_be_exported_hpp__
#define __split_module_to_be_exported_hpp__

namespace split_module{

struct op_struct{};

inline op_struct* get_opaque(){
    return 0;
}

inline void check_overload( int i=0, int j=1, int k=2 ){
}


struct item_t{

    enum EColor{ red, blue };
    enum EFruit{ apple, orange };

    item_t(){}
    item_t( int ){}

    void do_nothing(){}
    int do_something(){ return 1; }

    op_struct* get_opaque(){ return 0; }

    void check_overload( int i=0, int j=1, int k=2 ){}

    int m_dummy;

    struct nested_t{};
};
}


#endif//__split_module_to_be_exported_hpp__
