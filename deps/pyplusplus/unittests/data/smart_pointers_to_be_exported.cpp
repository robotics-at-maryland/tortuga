// Copyright 2004 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "smart_pointers_to_be_exported.hpp"

namespace smart_pointers{
   
data_a_ptr create_auto(){ return data_a_ptr( new data() ); }
data_s_ptr create_shared(){ return data_s_ptr( new data() ); }

int ref_auto( data_a_ptr& a ){ return a->get_value(); }
int ref_shared( data_s_ptr& a ){ return a->get_value(); }

int val_auto( data_a_ptr a ){ return a->get_value(); }
int val_shared( data_s_ptr a ){ return a->get_value(); }

int const_ref_auto( const data_a_ptr& a ){ return a->get_value(); }
int const_ref_shared( const data_s_ptr& a ){ return a->get_value(); }

int ref_auto_base_value( base_a_ptr& a ){ return a->get_base_value(); }
int ref_shared_base_value( base_s_ptr& a ){ return a->get_base_value(); }

int val_auto_base_value( base_a_ptr a ){ return a->get_base_value(); }
int val_shared_base_value( base_s_ptr a ){ return a->get_base_value(); }

int const_ref_auto_base_value( const base_a_ptr& a ){ return a->get_base_value(); }
int const_ref_shared_base_value( const base_s_ptr& a ){ return a->get_base_value(); }
   


int ref_auto_some_value( base_a_ptr& a ){ return a->get_some_value(); }
int ref_shared_some_value( base_s_ptr& a ){ return a->get_some_value(); }

int val_auto_some_value( base_a_ptr a ){ return a->get_some_value(); }
int val_shared_some_value( base_s_ptr a ){ return a->get_some_value(); }

int const_ref_auto_some_value( const base_a_ptr& a ){ return a->get_some_value(); }
int const_ref_shared_some_value( const base_s_ptr& a ){ return a->get_some_value(); }



}    
