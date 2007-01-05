# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""
This file contains C++ code - custom call policies
"""


namespace = "pyplusplus::call_policies"

file_name = "__call_policies.pypp.hpp"

code = \
"""// Copyright 2004 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef call_policies_pyplusplus_hpp__
#define call_policies_pyplusplus_hpp__

#include "boost/python.hpp"

namespace pyplusplus{ namespace call_policies{

namespace bpl = boost::python;

namespace detail{
    
struct make_value_holder{

    template <class T>
    static PyObject* execute(T* p){
        if (p == 0){
            return bpl::detail::none();
        }
        else{
            bpl::object p_value( *p );
            return bpl::incref( p_value.ptr() );
        }
    }

};

template <class R>
struct return_pointee_value_requires_a_pointer_return_type
# if defined(__GNUC__) && __GNUC__ >= 3 || defined(__EDG__)
{}
# endif
;

} //detail

struct return_pointee_value{

    template <class T>
    struct apply{
    
        BOOST_STATIC_CONSTANT( bool, ok = boost::is_pointer<T>::value );
        
        typedef typename boost::mpl::if_c<
            ok
            , bpl::to_python_indirect<T, detail::make_value_holder>
            , detail::return_pointee_value_requires_a_pointer_return_type<T>
        >::type type;
    
    };

};

template< typename CallPolicies, class T >
bpl::object make_object( T x ){
    //constructs object using CallPolicies result_converter
    typedef BOOST_DEDUCED_TYPENAME CallPolicies::result_converter:: template apply< T >::type result_converter_t;
    result_converter_t rc;
    return bpl::object( bpl::handle<>( rc( x ) ) );
}

} /*pyplusplus*/ } /*call_policies*/


#endif//call_policies_pyplusplus_hpp__

"""
