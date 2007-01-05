// Copyright 2004 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __smart_pointers_to_be_exported_hpp__
#define __smart_pointers_to_be_exported_hpp__
#include <memory>
#include "boost/shared_ptr.hpp"

namespace smart_pointers{

struct base{
    base() : base_value(19) {}
    int base_value;
    virtual int get_base_value(){ return base_value; }
    virtual int get_some_value() = 0;
};

struct data : base{
    data() : value(11){}
    int value;
    virtual int get_value(){ return value; }
    virtual int get_some_value(){ return 23; }
};

typedef std::auto_ptr< base > base_a_ptr;
typedef boost::shared_ptr< base > base_s_ptr;

typedef std::auto_ptr< data > data_a_ptr;
typedef boost::shared_ptr< data > data_s_ptr;
    
data_a_ptr create_auto();
data_s_ptr create_shared();

int ref_auto( data_a_ptr& a );
int ref_shared( data_s_ptr& a );

int val_auto( data_a_ptr a );
int val_shared( data_s_ptr a );

int const_ref_auto( const data_a_ptr& a );
int const_ref_shared( const data_s_ptr& a );

int ref_auto_base_value( base_a_ptr& a );
int ref_shared_base_value( base_s_ptr& a );

int val_auto_base_value( base_a_ptr a );
int val_shared_base_value( base_s_ptr a );

int const_ref_auto_base_value( const base_a_ptr& a );
int const_ref_shared_base_value( const base_s_ptr& a );

int ref_auto_some_value( base_a_ptr& a );
int ref_shared_some_value( base_s_ptr& a );

int val_auto_some_value( base_a_ptr a );
int val_shared_some_value( base_s_ptr a );

int const_ref_auto_some_value( const base_a_ptr& a );
int const_ref_shared_some_value( const base_s_ptr& a );

struct shared_data_buffer_t{
    shared_data_buffer_t() 
    : size( 0 )
    {}
    int size;
};

struct shared_data_buffer_holder_t{
    typedef boost::shared_ptr<shared_data_buffer_t> holder_impl_t;
    shared_data_buffer_holder_t()
    : buffer( new shared_data_buffer_t() )
      , const_buffer( new shared_data_buffer_t() )
    {}
        
    holder_impl_t buffer;
    const holder_impl_t const_buffer;
};

}    

#endif//__smart_pointers_to_be_exported_hpp__
