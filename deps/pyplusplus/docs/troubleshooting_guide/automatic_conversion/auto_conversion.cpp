#include "boost/python.hpp"
#include "boost/python/object.hpp" //len function
#include "boost/python/ssize_t.hpp" //ssize_t type definition
#include <string>

struct point3d_t{
    point3d_t() 
    : x(0), y(0), z(0)
    {}
        
    int x, y, z;
};

namespace bpl = boost::python;

namespace point3d_conversion{
    
struct to_tuple{

    to_tuple(){
        bpl::to_python_converter< point3d_t, to_tuple>();
    }
    
    static PyObject* convert(point3d_t const& pt){
        return bpl::incref( bpl::make_tuple( pt.x, pt.y, pt.z ).ptr() );
    }

};

struct from_tuple{
    
    static void* convertible(PyObject* py_seq){
        if( !PySequence_Check( py_seq ) ){ 
            //check that the argument is a sequence
            return 0;
        }
        bpl::object seq = bpl::object( bpl::handle<>( py_seq ) );
        bpl::ssize_t size = bpl::len( seq );
        if( 3 != size ){
            return 0;
        }
        for( bpl::ssize_t i = 0; i < 3; ++i ){
            //test that every item in sequence has int type
            bpl::object item = seq[i];
            bpl::extract<int> type_checker( item );
            if( !type_checker.check() ){
                return 0;
            }
        }       
        return py_seq;
    }

    //rvalue_from_python_stage1_data is a class defined in
    //boost/python/converter/rvalue_from_python_data.hpp file. The file contains
    //nice and not "very" technical explanation about data management for rvalue 
    //conversions from Python to C++ types. 
    
    //In my words, the rvalue_from_python_stage1_data class contains memory 
    //needed for construction of appropriate C++ object. The class also manages
    //the life time of the constructed C++ object.
    
    static void 
    construct( PyObject* py_seq, bpl::converter::rvalue_from_python_stage1_data* data){
        if( !convertible( py_seq ) ){
            bpl::throw_error_already_set();
        }

        //At this point you are exposed to low level implementation details
        //of Boost.Python library. You use the reinterpret_cast, in order to get
        //access to the number of bytes, needed to store C++ object        
        typedef bpl::converter::rvalue_from_python_storage<point3d_t> point3d_storage_t;
        point3d_storage_t* the_storage = reinterpret_cast<point3d_storage_t*>( data );
        //Now we need to get access to the memory, which will held the object.
        void* memory_chunk = the_storage->storage.bytes;
        //You should use placement new in order to create object in specific
        //location
        point3d_t* point = new (memory_chunk) point3d_t();
        //We allocated the memory, now we should tell Boost.Python to manage( free )
        //it later
        data->convertible = memory_chunk;

        bpl::object seq = bpl::object( bpl::handle<>( py_seq ) );        
        //Now I actually creates the object from the Python tuple
        bpl::object tmp = seq[0];
        point->x = bpl::extract< int >( tmp );
        tmp = seq[1];
        point->y = bpl::extract< int >( tmp );
        tmp = seq[2];
        point->z = bpl::extract< int >( tmp );
    }
    
};

void register_conversion(){
    
    bpl::to_python_converter< point3d_t, to_tuple>();
    
    bpl::converter::registry::push_back(  &from_tuple::convertible
                                        , &from_tuple::construct
                                        , bpl::type_id<point3d_t>() );
}

}//namespace point3d_conversion

point3d_t zero_point() { 
    return point3d_t(); 
}


BOOST_PYTHON_MODULE( auto_conversion ){
    point3d_conversion::register_conversion();
    bpl::def("zero_point", &::zero_point);
}

