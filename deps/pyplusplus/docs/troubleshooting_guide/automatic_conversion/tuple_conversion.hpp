#ifndef tuple_conversion_hpp
#define tuple_conversion_hpp

#include "boost/python.hpp"
#include "boost/tuple/tuple.hpp"
#include "boost/python/object.hpp" //len function
#include <boost/mpl/int.hpp>
#include <boost/mpl/next.hpp>

/*******************************************************************************
 
This file contains conversion from boost::tuples::tuple class instantiation to
Python tuple and vice versa. The conversion uses Boost.Python custom r-value 
converters. Custom r-value converters allows Boost.Python library to handle
[from|to] [C++|Python] variables conversion "on-the-fly". For example:

typedef boost::tupls::tuple< int, std::string > record_t;

record_t read_record();
bool validate_record(record_t const & r);

If you don't use custom r-value converters than you need to create small wrappers
around the function or expose record_t class. 

**/

namespace boost{ namespace python{ 

namespace details{
    
//Small helper function, introduced to allow short syntax for index incrementing
template< int index>    
typename mpl::next< mpl::int_< index > >::type increment_index(){
    typedef typename mpl::next< mpl::int_< index > >::type next_index_type;
    return next_index_type();
}   

}
    
/*******************************************************************************

Conversion from C++ type to Python type is pretty simple. Basically, using 
Python C API you create an object and than initialize it.

**/
template< class TTuple >    
struct to_py_tuple{
    
    typedef mpl::int_< tuples::length< TTuple >::value > length_type;
    
    static PyObject* convert(const TTuple& c_tuple){
        list values;
        //add all c_tuple items to "values" list
        convert_impl( c_tuple, values, mpl::int_< 0 >(), length_type() );
        //create Python tuple from the list
        return incref( python::tuple( values ).ptr() );
    }
    
private:

    template< int index, int length >
    static void 
    convert_impl( const TTuple &c_tuple, list& values, mpl::int_< index >, mpl::int_< length > ) {
        values.append( c_tuple.template get< index >() );
        convert_impl( c_tuple, values, details::increment_index<index>(), length_type() );
    }

    template< int length >
    static void 
    convert_impl( const TTuple&, list& values, mpl::int_< length >, mpl::int_< length >)
    {}

};


/*******************************************************************************

Conversion from Python type to C++ type is a little bit complex. Boost.Python
library implements solution, which manages the memory of the allocated object.
This was done in order to allow 
In order to implement "from Python" conversion you should supply 2 functions.
The first one checks whether the conversion is possible. The second function 
should construct an instance of the desired class. 

**/

template< class TTuple>
struct from_py_tuple{
        
    typedef mpl::int_< tuples::length< TTuple >::value > length_type;
    
    static void* 
    convertible(PyObject* py_obj){
        
        if( !PyTuple_Check( py_obj ) ){ 
            return 0;
        }
        
        python::tuple py_tuple( handle<>( borrowed( py_obj ) ) );
        if( tuples::length< TTuple >::value != len( py_tuple ) ){
            return 0;
        }

        if( convertible_impl( py_tuple, mpl::int_< 0 >(), length_type() ) ){
            return py_obj;
        }
        else{
            return 0;
        }
    }

    static void
    construct( PyObject* py_obj, converter::rvalue_from_python_stage1_data* data){
        //At this point you are exposed to low level implementation details
        //of Boost.Python library. You use the reinterpret_cast, in order to get
        //access to the number of bytes, needed to store C++ object        
        typedef converter::rvalue_from_python_storage<TTuple> storage_t;
        storage_t* the_storage = reinterpret_cast<storage_t*>( data );
        //Now we need to get access to the memory, which will held the object.
        void* memory_chunk = the_storage->storage.bytes;
        //You should use placement new in order to create object in specific
        //location
        TTuple* c_tuple = new (memory_chunk) TTuple();
        //We allocated the memory, now we should tell Boost.Python to manage( free )
        //it later
        data->convertible = memory_chunk;

        python::tuple py_tuple( handle<>( borrowed( py_obj ) ) );
        construct_impl( py_tuple, *c_tuple, mpl::int_< 0 >(), length_type() );
    }
    
private:
    
    template< int index, int length >
    static bool 
    convertible_impl( const python::tuple& py_tuple, mpl::int_< index >, mpl::int_< length > ){
	
        typedef typename tuples::element< index, TTuple>::type element_type;
        
        object element = py_tuple[index];
        extract<element_type> type_checker( element );
        if( !type_checker.check() ){
            return false;
        }
        else{
            return convertible_impl( py_tuple, details::increment_index<index>(), length_type() );
        }            
    }

    template< int length >
    static bool
    convertible_impl( const python::tuple& py_tuple, mpl::int_< length >, mpl::int_< length > ){
        return true;
    }
    
    template< int index, int length >
    static void
    construct_impl( const python::tuple& py_tuple, TTuple& c_tuple, mpl::int_< index >, mpl::int_< length > ){
	
        typedef typename tuples::element< index, TTuple>::type element_type;
        
        object element = py_tuple[index];
        c_tuple.template get< index >() = extract<element_type>( element );

        construct_impl( py_tuple, c_tuple, details::increment_index<index>(), length_type() );
    }

    template< int length >
    static void
    construct_impl( const python::tuple& py_tuple, TTuple& c_tuple, mpl::int_< length >, mpl::int_< length > )
    {}

};
    
template< class TTuple>
void register_tuple(){
    
    to_python_converter< TTuple, to_py_tuple<TTuple> >();
        
    converter::registry::push_back( &from_py_tuple<TTuple>::convertible
                                    , &from_py_tuple<TTuple>::construct
                                    , type_id<TTuple>() );
};

} } //boost::python

#endif//tuple_conversion_hpp
