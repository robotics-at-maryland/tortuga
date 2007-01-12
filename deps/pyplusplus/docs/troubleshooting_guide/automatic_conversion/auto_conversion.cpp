#include "boost/python.hpp"
#include "boost/python/object.hpp" //len function
#include "boost/python/ssize_t.hpp" //ssize_t type definition
#include "boost/python/detail/none.hpp"

//This example is very similar to one found in Boost.Python FAQs:
//How can I automatically convert my custom string type to and from a Python string?
//http://www.boost.org/libs/python/doc/v2/faq.html#custom_string
//I will introduce another example with detailed explanation. 

//Simple class, which describes a point in 3D. I don't want to export the class.
//Instead of this I want Python user pass tuple as argument to all functions
//that expects point3d_t class instance and Boost.Python will automaticly handle 
//the conversion. Same conversion should be applied on functions, which returns
//point3d_t class instance

struct point3d_t{
    explicit point3d_t(int x_=0, int y_=0, int z_=0) 
    : x(x_), y(y_), z(z_)
    {}
    
    bool operator==( const point3d_t& other ) const{
        return x == other.x && y == other.y && z == other.z;
    }
    
    int x, y, z;
};


point3d_t point_ret_val_000() { 
    return point3d_t(); 
}

point3d_t point_ret_val_101() { 
    return point3d_t(1,0,1); 
}

point3d_t& point_ret_ref_010(){
    static point3d_t pt( 0,1,0 );
    return pt;
}

point3d_t* point_ret_ptr_110(){
    static point3d_t pt( 1,1,0 );
    return &pt;
}

bool test_point_val_000( point3d_t pt ){
    return pt == point3d_t( 0,0,0 );
}

bool test_point_cref_010( const point3d_t& pt ){
    return pt == point3d_t( 0,1,0 );
}

bool test_point_ref_110( point3d_t& pt ){
    return pt == point3d_t( 1,1,0 );
}

bool test_point_ptr_101( point3d_t* pt ){
    return pt && *pt == point3d_t( 1,0,1 );
}


namespace bpl = boost::python;

namespace point3d_conversion{
    
struct to_tuple{
    //"To Python" conversion is pretty simple:
    //Using Python C API create an object and than manually fill it with data.
    //In this case Boost.Python provides a convenience API for tuple construction.
    //I don't see any reason not to use it.
    static PyObject* convert(point3d_t const& pt){
        return bpl::incref( bpl::make_tuple( pt.x, pt.y, pt.z ).ptr() );
    }
};

struct from_tuple{
    //"From Python" conversion is more complex. Memory managment is the main 
    //reason for this.

    //The first step in conversion from the Python object is to check whether
    //the object is the right one.
    static void* convertible(PyObject* py_seq){
        if( !PySequence_Check( py_seq ) ){ 
            //check that the argument is a sequence
            return 0;
        }
        bpl::object seq = bpl::object( bpl::handle<>( bpl::borrowed( py_seq ) ) );
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

        bpl::object seq = bpl::object( bpl::handle<>( bpl::borrowed( py_seq ) ) );        
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

BOOST_PYTHON_MODULE( auto_conversion ){
    point3d_conversion::register_conversion();
    bpl::def("point_ret_val_000", &::point_ret_val_000);
    bpl::def("point_ret_val_101", &::point_ret_val_101);
    bpl::def("point_ret_ref_010"
             , &::point_ret_ref_010
             , bpl::return_value_policy<bpl:: copy_non_const_reference>() );
    bpl::def( "point_ret_ptr_110"
              , &::point_ret_ptr_110
              , bpl::return_value_policy<bpl::return_by_value>() );
    bpl::def("test_point_val_000", &::test_point_val_000);
    bpl::def("test_point_cref_010", &::test_point_cref_010);
    bpl::def("test_point_ref_110", &::test_point_ref_110);
    bpl::def("test_point_ptr_101", &::test_point_ptr_101);
}

