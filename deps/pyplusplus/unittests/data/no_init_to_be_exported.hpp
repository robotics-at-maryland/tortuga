// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __no_init_to_be_exported_hpp__
#define __no_init_to_be_exported_hpp__

#include "boost/shared_ptr.hpp"

namespace no_init_ns{

struct value_i{
public:
    virtual ~value_i() { }
    virtual int get_value(void) const = 0;
};

struct value_plus_x_t : value_i{
    value_plus_x_t( int value )
    : m_value( value )
    {}

    virtual int get_value(void) const{
        return  m_value + get_plus_value();
    }

    virtual int get_plus_value() const{
        return 0;
    }

private:
    int m_value;
};

inline int
get_value( const boost::shared_ptr< value_i >& v ){
    if( v ){
        return v->get_value();
    }
    else{
        return -1;
    }
}

struct event_t{
    template< class TData >
    event_t( TData& ){};
};

struct map_iterator_t{
    map_iterator_t( int i ){}
    map_iterator_t( const map_iterator_t& ){}
private:
    map_iterator_t(){};
};
/*
class Data{
private:
    Data(){}
};

class Value : public Data{
public:
    int get1() { return 1; }
};

class Primitive : public Value
{};

class Boolean : public Primitive{
    bool value;
};
*/
}

#endif//__no_init_to_be_exported_hpp__
