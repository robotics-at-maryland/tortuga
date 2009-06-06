// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __split_module_to_be_exported_hpp__
#define __split_module_to_be_exported_hpp__

#include "boost/shared_ptr.hpp"
//#include BOOST_HASH_MAP_HEADER
#include <map>
#include <vector>
#include <string>

namespace split_module{

typedef std::vector< std::vector< int > > naive_matrix_t;

class TestClass1 {

public:
   int func() {
      return 1;
   }

};

class TestClass2 {

public:
   int func() {
      return 2;
   }

};

class TestClass3 {

public:
   int func() {
      return 3;
   }
};


inline naive_matrix_t create_zero_matrix( unsigned int x ){
    return naive_matrix_t();
};


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

//typedef BOOST_STD_EXTENSION_NAMESPACE::hash_map< std::string, boost::shared_ptr< item_t > > str2item_t;
typedef std::map< std::string, boost::shared_ptr< item_t > > str2item_t;
inline str2item_t create_empty_mapping(){
    return str2item_t();
}

}


#endif//__split_module_to_be_exported_hpp__
