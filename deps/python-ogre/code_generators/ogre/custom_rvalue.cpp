#include "boost/python.hpp"
#include "boost/python/object.hpp"  //len function
#include "boost/python/ssize_t.hpp" //ssize_t type definition
#include "boost/python/detail/none.hpp"
#include "tuples.hpp"
#include "OgreColourValue.h"
#include "OgreVector2.h"
#include "OgreVector3.h"
#include "OgreVector4.h"
#include "OgreUTFString.h"

namespace bpl = boost::python;

namespace r_values_impl{

struct PyTuple2ColourValue{

    typedef boost::tuples::tuple< float, float, float> rgb_type;
    typedef boost::tuples::tuple< float, float, float, float> rgba_type;

    typedef bpl::from_py_sequence< rgb_type > rgb_converter_type;
    typedef bpl::from_py_sequence< rgba_type > rgba_converter_type;

    typedef Ogre::ColourValue colour_t;

    static void* convertible(PyObject* obj){
        if( rgb_converter_type::convertible( obj ) || rgba_converter_type::convertible( obj ) ){
            return obj;
        }
        else{
            return 0;
        }
    }

    static void
    construct( PyObject* obj, bpl::converter::rvalue_from_python_stage1_data* data){

        typedef bpl::converter::rvalue_from_python_storage<colour_t> colour_storage_t;
        colour_storage_t* the_storage = reinterpret_cast<colour_storage_t*>( data );
        void* memory_chunk = the_storage->storage.bytes;

        float red(1.0), green(1.0), blue(1.0), alpha(1.0);
        bpl::tuple py_tuple( bpl::handle<>( bpl::borrowed( obj ) ) );
        if( 3 == bpl::len( py_tuple ) ){
            boost::tuples::tie(red, green, blue)
                = rgb_converter_type::to_c_tuple( obj );
        }
        else{
            boost::tuples::tie(red, green, blue, alpha) 
                = rgba_converter_type::to_c_tuple( obj );
        }

        colour_t* colour = new (memory_chunk) colour_t(red, green, blue, alpha);
        data->convertible = memory_chunk;
    }
};

typedef Ogre::Real real_type;

struct PyTuple2Vector2{

    typedef Ogre::Vector2 vector_type;

    typedef boost::tuples::tuple< real_type, real_type> vector_tuple_type;

    typedef bpl::from_py_sequence< vector_tuple_type > converter_type;

    static void* convertible(PyObject* obj){
        return converter_type::convertible( obj );
    }

    static void
    construct( PyObject* obj, bpl::converter::rvalue_from_python_stage1_data* data){

        typedef bpl::converter::rvalue_from_python_storage<vector_type> vector_storage_t;
        vector_storage_t* the_storage = reinterpret_cast<vector_storage_t*>( data );
        void* memory_chunk = the_storage->storage.bytes;

        real_type x(0.0), y(0.0);
        boost::tuples::tie(x, y) = converter_type::to_c_tuple( obj );

        vector_type* vec = new (memory_chunk) vector_type(x,y);
        data->convertible = memory_chunk;
    }
};

struct PyTuple2Vector3{

    typedef Ogre::Vector3 vector_type;

    typedef boost::tuples::tuple< real_type, real_type, real_type> vector_tuple_type;

    typedef bpl::from_py_sequence< vector_tuple_type > converter_type;

    static void* convertible(PyObject* obj){
        return converter_type::convertible( obj );
    }

    static void
    construct( PyObject* obj, bpl::converter::rvalue_from_python_stage1_data* data){

        typedef bpl::converter::rvalue_from_python_storage<vector_type> vector_storage_t;
        vector_storage_t* the_storage = reinterpret_cast<vector_storage_t*>( data );
        void* memory_chunk = the_storage->storage.bytes;

        real_type x(0.0), y(0.0), z(0.0);
        boost::tuples::tie(x, y, z) = converter_type::to_c_tuple( obj );

        vector_type* vec = new (memory_chunk) vector_type(x, y, z);
        data->convertible = memory_chunk;
    }
};


struct PyTuple2Vector4{

    typedef Ogre::Vector4 vector_type;

    typedef boost::tuples::tuple< real_type, real_type, real_type, real_type> vector_tuple_type;

    typedef bpl::from_py_sequence< vector_tuple_type > converter_type;

    static void* convertible(PyObject* obj){
        return converter_type::convertible( obj );
    }

    static void
    construct( PyObject* obj, bpl::converter::rvalue_from_python_stage1_data* data){

        typedef bpl::converter::rvalue_from_python_storage<vector_type> vector_storage_t;
        vector_storage_t* the_storage = reinterpret_cast<vector_storage_t*>( data );
        void* memory_chunk = the_storage->storage.bytes;

        real_type x(0.0), y(0.0), z(0.0), w(0.0);
        boost::tuples::tie(x, y, z, w) = converter_type::to_c_tuple( obj );

        vector_type* vec = new (memory_chunk) vector_type(x, y, z, w);
        data->convertible = memory_chunk;
    }
};


struct PyStringToUTFString{

    typedef Ogre::UTFString string_type;

    static void* convertible(PyObject* obj){
        if( !PyString_Check(obj) || !PyUnicode_Check(obj) ){
            return 0;
        }
        return obj;
    }

    static void
    construct( PyObject* obj, bpl::converter::rvalue_from_python_stage1_data* data){
        typedef bpl::converter::rvalue_from_python_storage<string_type> string_storage_t;
        string_storage_t* the_storage = reinterpret_cast<string_storage_t*>( data );
        void* memory_chunk = the_storage->storage.bytes;

        bpl::object py_str( bpl::handle<>( bpl::borrowed( obj ) ) );                
        if( PyString_Check(obj) ){        
            std::string c_str = bpl::extract<std::string>( py_str );
            new (memory_chunk) string_type(c_str);
        }
        else{ //unicode
            std::wstring c_str = bpl::extract<std::wstring>( py_str );
            new (memory_chunk) string_type(c_str);
        }
        data->convertible = memory_chunk;        
    }
};


} //r_values_impl


void register_pytuple_to_colour_value_conversion(){
    bpl::converter::registry::push_back(  &r_values_impl::PyTuple2ColourValue::convertible
                                         , &r_values_impl::PyTuple2ColourValue::construct
                                         , bpl::type_id<Ogre::ColourValue>() );
}

void register_pytuple_to_vector2_conversion(){
    bpl::converter::registry::push_back(  &r_values_impl::PyTuple2Vector2::convertible
                                         , &r_values_impl::PyTuple2Vector2::construct
                                         , bpl::type_id<Ogre::Vector2>() );
}

void register_pytuple_to_vector3_conversion(){
    bpl::converter::registry::push_back(  &r_values_impl::PyTuple2Vector3::convertible
                                         , &r_values_impl::PyTuple2Vector3::construct
                                         , bpl::type_id<Ogre::Vector3>() );
}

void register_pytuple_to_vector4_conversion(){
    bpl::converter::registry::push_back(  &r_values_impl::PyTuple2Vector4::convertible
                                         , &r_values_impl::PyTuple2Vector4::construct
                                         , bpl::type_id<Ogre::Vector4>() );
}

void register_pystring_to_utfstring_conversion(){
    bpl::converter::registry::push_back(  &r_values_impl::PyStringToUTFString::convertible
                                         , &r_values_impl::PyStringToUTFString::construct
                                         , bpl::type_id<Ogre::UTFString>() );
}

