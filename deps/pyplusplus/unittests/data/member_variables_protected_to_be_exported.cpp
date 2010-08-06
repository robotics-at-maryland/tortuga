// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "member_variables_protected_to_be_exported.hpp"

namespace member_variables{

int point::instance_count = 0;
const point::color point::default_color = point::red;

unsigned int get_a(const bit_fields_t& inst){
    return inst.a;
}

void set_a( bit_fields_t& inst, unsigned int new_value ){
    inst.a = new_value;
}

unsigned int get_b(const bit_fields_t& inst){
    return inst.b;
}

namespace pointers{

std::auto_ptr<tree_node_t> create_tree(){
    std::auto_ptr<tree_node_t> root( new tree_node_t() );
    root->data = new data_t();
    root->data->value = 0;

    root->left = new tree_node_t( root.get() );
    root->left->data = new data_t();
    root->left->data->value = 1;

    return root;
}

}

namespace statics{
    std::string mem_var_str_t::class_name( "mem_var_str_t" );
}


namespace ctypes{
    int xxx = 1997;
    int* image_t::none_image = &xxx;
}

}
