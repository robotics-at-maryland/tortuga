// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __ft_from_address_to_be_exported_hpp__
#define __ft_from_address_to_be_exported_hpp__

#include <stdexcept>

inline unsigned long
sum_matrix( unsigned int* matrix, unsigned int rows, unsigned int columns ){
    if( !matrix ){
        throw std::runtime_error( "matrix is null" );
    }
    unsigned long result = 0;
    for( unsigned int r = 0; r < rows; ++r ){
        for( unsigned int c = 0; c < columns; ++c ){
            result += *matrix;
            ++matrix;
        }
    }
    return result;
}

struct ptr_ptr_t{

    ptr_ptr_t()
    : value( new double( 5.9 ) )
    {}

    ~ptr_ptr_t(){
        delete value;
    }

    void get_v_address( double** v ){
        if( !v ){
            throw std::runtime_error( "null pointer" );
            *v = value;
        }
    }

    double* value;
};

#endif//__ft_from_address_to_be_exported_hpp__
