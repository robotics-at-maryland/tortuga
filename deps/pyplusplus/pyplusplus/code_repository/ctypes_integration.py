# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""
This file contains C++ code needed to export one dimensional static arrays.
"""


namespace = "pyplusplus::convenience"

file_name = "__ctypes_integration.pypp.hpp"

code = \
"""// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __ctypes_integration_pyplusplus_hpp__
#define __ctypes_integration_pyplusplus_hpp__

#include "boost/python.hpp"
#include "boost/utility/addressof.hpp"
#include "boost/mpl/vector.hpp"
#include "boost/function.hpp"
#include "boost/cstdint.hpp"
#include "boost/type.hpp"
#include "boost/bind.hpp"


namespace pyplusplus{ namespace convenience{

template< typename TType, typename TMemVarType >
boost::uint32_t
addressof( const TType* inst_ptr, const TMemVarType TType::* offset){
    if( !inst_ptr ){
        throw std::runtime_error( "unable to dereference null pointer" );
    }
    const TType& inst = *inst_ptr;
    return boost::uint32_t( boost::addressof( inst.*offset ) );
}

template< typename TType >
boost::uint32_t
addressof_inst( const TType* inst_ptr){
    if( !inst_ptr ){
        throw std::runtime_error( "unable to dereference null pointer" );
    }

    return boost::uint32_t( inst_ptr );
}

template< typename TType, typename TMemVarType >
boost::python::object
make_addressof_getter( const TMemVarType TType::* offset ){
    namespace bpl = boost::python;
    namespace pyppc = pyplusplus::convenience;
    return bpl::make_function( boost::bind( &pyppc::addressof< TType, TMemVarType >, _1, offset )
                               , bpl::default_call_policies()
                               , boost::mpl::vector< boost::uint32_t, const TType* >() );
}

template< typename TType >
boost::python::object
make_addressof_inst_getter(){
    namespace bpl = boost::python;
    namespace pyppc = pyplusplus::convenience;
    return bpl::make_function( boost::bind( &pyppc::addressof_inst< TType >, _1 )
                               , bpl::default_call_policies()
                               , boost::mpl::vector< boost::uint32_t, const TType* >() );
}

class register_addressof_static_var : public boost::python::def_visitor<register_addressof_static_var>
{
    friend class boost::python::def_visitor_access;

public:

    template< typename TVarType >
    register_addressof_static_var( const char* name, const TVarType& var )
    : m_name( name )
      , m_address( addressof_inst( boost::addressof( var ) ) )
    {}

    template <class classT>
    void visit(classT& c) const{
        boost::python::scope cls_scope( c );
        cls_scope.attr(m_name) = m_address;
    }

private:
    boost::uint32_t m_address;
    const char* m_name;
};


class register_sizeof : public boost::python::def_visitor<register_sizeof>
{
    friend class boost::python::def_visitor_access;

public:

    template< typename TType >
    register_sizeof( boost::type< TType > )
    : m_sizeof( sizeof( TType ) )
    {}

    template <class classT>
    void visit(classT& c) const{
        boost::python::scope cls_scope( c );
        cls_scope.attr("sizeof") = m_sizeof;
    }

private:
    size_t m_sizeof;
};


} /*pyplusplus*/ } /*convenience*/

namespace pyplus_conv = pyplusplus::convenience;

#endif//__ctypes_integration_pyplusplus_hpp__

"""

