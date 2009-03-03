// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __return_auto_ptr_to_be_exported_hpp__
#define __return_auto_ptr_to_be_exported_hpp__
#include "boost/shared_ptr.hpp"
#include <memory>


namespace smart_pointers{

struct r_input_t{
    r_input_t(unsigned int rows, unsigned int cols)
    :  m_rows(rows)
       , m_cols(cols)
    {}
        
    unsigned int rows() const {return m_rows;}
    unsigned int cols() const {return m_cols;}

private:
    unsigned int m_rows;
    unsigned int m_cols;
};


class generic_input_t
{
public:
    // Pure virtual function to convert to the required type
    virtual std::auto_ptr<r_input_t> create_r_input() = 0;

    virtual boost::shared_ptr<r_input_t> create_r_input_shared() = 0;
};


// Function which converts generic_input_t to r_input_t
std::auto_ptr<r_input_t> process_input(generic_input_t& input)
{
    return input.create_r_input();
}

boost::shared_ptr<r_input_t> process_input_shared(generic_input_t& input)
{
    return input.create_r_input_shared();
}

}    

#endif//__return_auto_ptr_to_be_exported_hpp__
