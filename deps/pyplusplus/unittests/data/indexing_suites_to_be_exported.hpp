// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __indexing_suites_to_be_exported_hpp__
#define __indexing_suites_to_be_exported_hpp__

#include <vector>
#include <string>

namespace indexing_suites {

typedef std::vector< std::string > strings_t;

inline void do_nothing( const strings_t& ){}

inline strings_t get_names(){
    strings_t names;
    names.push_back( "a" );
    names.push_back( "a" );
    names.push_back( "a" );
    return names;
}

struct item_t{    
    item_t() : value( -1 ){}
    
    bool operator==(item_t const& item) const { 
        return value == item.value; 
    }
    
    bool operator!=(item_t const& item) const { 
        return value != item.value; 
    }    
    
    int value;
};


typedef std::vector<item_t> items_t;

typedef std::vector<item_t*> items_ptr_t;

inline item_t get_value( const std::vector<item_t>& vec, unsigned int index ){
    return vec.at(index);
}

inline item_t get_ptr_value( const std::vector<item_t*>& vec, unsigned int index ){
    return *vec.at(index);
}

inline void set_value( std::vector<item_t>& vec, unsigned int index, item_t value ){
    vec.at(index);
    vec[index] = value;
}

typedef std::vector<int> ivector;
ivector empty_ivector(){ return ivector(); }

}

#endif//__indexing_suites_to_be_exported_hpp__
