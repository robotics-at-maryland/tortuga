# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base


MODULE_SPTR_DECL_CODE = \
"""
# include <boost/python/converter/from_python.hpp>
# include <boost/python/converter/rvalue_from_python_data.hpp>
# include <boost/python/converter/registered.hpp>

namespace boost{ namespace python{

    using namespace controllers;
    using namespace converter;
    controller_i* get_pointer( my_smart_ptr_t< controller_i > const& p ){
        return p.get();
    }

    template <>
    struct pointee< my_smart_ptr_t< controller_i > >{
        typedef controller_i type;
    };


    add_x_t* get_pointer( my_smart_ptr_t< add_x_t > const& p ){
        return p.get();
    }

    template <>
    struct pointee< my_smart_ptr_t< add_x_t > >{
        typedef add_x_t type;
    };


template <class T>
struct my_smart_ptr_from_python
{
    my_smart_ptr_from_python()
    {
        converter::registry::insert(&convertible, &construct, type_id<my_smart_ptr_t<T> >());
    }

 private:
    static void* convertible(PyObject* p)
    {
        if (p == Py_None)
            return p;
        
        return converter::get_lvalue_from_python(p, registered<T>::converters);
    }
    
    static void construct(PyObject* source, rvalue_from_python_stage1_data* data)
    {
        void* const storage = ((converter::rvalue_from_python_storage<my_smart_ptr_t<T> >*)data)->storage.bytes;
        // Deal with the "None" case.
        if (data->convertible == source)
            new (storage) my_smart_ptr_t<T>();
        else{
            std::cout << "before new (storage) my_smart_ptr_t<T>( static_cast< T* >(data->convertible) );" << std::endl;
            new (storage) my_smart_ptr_t<T>( static_cast< T* >(data->convertible) );
        }
        data->convertible = storage;
    }
};

    template <class T>
    struct ptr_to_python {
        static PyObject *convert(my_smart_ptr_t<T> const &p) {
            return incref(object(my_smart_ptr_t<T>(p)).ptr());
        }
    };

    //Then you need to do this for each class you register:
    //to_python_converter<ptr<T>, ptr_to_python<T> >();
    //You could automate this in a custom def visitor:

    struct smart_ptr_stuff : def_visitor<smart_ptr_stuff> {
        friend class def_visitor_access;
        template <typename Class>
        void visit(Class &c) const {
            typedef typename Class::wrapped_type T;
            to_python_converter<my_smart_ptr_t<T>, ptr_to_python<T> >();
        }
    };


}}// namespace boost::python converter
"""

MODULE_SPTR_REG_CODE = \
"""
    boost::python::my_smart_ptr_from_python<controllers::add_x_t>();
    
    bp::register_ptr_to_python< my_smart_ptr_t< controllers::controller_i > >();

    //bp::register_ptr_to_python< my_smart_ptr_t< controllers::add_x_t > >();

    bp::implicitly_convertible< my_smart_ptr_t< controllers::add_x_t >, my_smart_ptr_t< controllers::controller_i > >();

"""

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'custom_smart_ptr_classes'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize( self, mb ):
        mb.classes( lambda cls: 'ptr' in cls.name).exclude()
        mb.add_declaration_code( MODULE_SPTR_DECL_CODE )
        mb.add_registration_code( MODULE_SPTR_REG_CODE )
        add_x_t = mb.class_( 'add_x_t' )
        add_x_t.held_type = 'my_smart_ptr_t< controllers::add_x_t >'
        add_x_t.add_registration_code( 'def(boost::python::smart_ptr_stuff())' )

        mb.build_code_creator( self.EXTENSION_NAME )
        mb.code_creator.add_include( 'iostream' )
        
        
    def create_identity_value( self, module, v ):
        class identity_value_t( module.value_i ):
            def __init__( self, v ):
                module.value_i.__init__( self )
                self.value = v

            def get_value(self):
                return self.value

        return identity_value_t(v)

    def create_add_5_value( self, module, v ):
        class add_5_value_t( module.value_plus_x_t ):
            def __init__( self, v ):
                module.value_plus_x_t.__init__( self, v )

            def get_plus_value(self):
                print "I am here"
                return 5
        return add_5_value_t( v )

    def run_tests(self, module):
        add_0 = module.add_x_t( 23 )
        print '>',1
        self.failUnless( 23 == add_0.get_value() )
        print '>',2
        self.failUnless( 23 == module.val_get_value( add_0 ) )
        print '>',3
        self.failUnless( 23 == module.const_ref_get_value( add_0 ) )
        print '>',4
        self.failUnless( 23 == module.ref_get_value( add_0 ) )


def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
