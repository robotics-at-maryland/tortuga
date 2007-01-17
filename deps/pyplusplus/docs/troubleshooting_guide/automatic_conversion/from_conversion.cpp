#include "boost/python.hpp"
#include "boost/python/object.hpp" //len function
#include "boost/python/ssize_t.hpp" //ssize_t type definition
#include "boost/python/detail/none.hpp"


struct colour_t{
    explicit colour_t( float red_=0.0, float green_=0.0, float blue_=0.0 )
    : red( red_ ), green( green_ ), blue( blue_ )
    {}
    
    bool operator==( const colour_t& other ) const{
        return red == other.red && green == other.green && blue == other.blue;
    }
        
    float red, green, blue;
};

struct image_t{
    colour_t background;
    bool test_background( const colour_t& color )const{
        return color == background;
    }
};

bool test_val_010( colour_t colour ){
    return colour == colour_t( 0, 1, 0);
}

bool test_cref_000( const colour_t& colour ){
    return colour == colour_t( 0, 0, 0);
}

bool test_ref_111( const colour_t& colour ){
    return colour == colour_t( 1, 1, 1);
}

bool test_ptr_101( colour_t* colour ){
    return colour && *colour == colour_t( 1, 0, 1);
}

bool test_cptr_110( const colour_t* colour ){
    return colour && *colour == colour_t( 1, 1, 0);
}


namespace bpl = boost::python;

namespace colour_conversion{
    
struct from_tuple{

    static void* convertible(PyObject* py_seq){
        if( !PySequence_Check( py_seq ) ){ 
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
            bpl::extract<float> type_checker( item );
            if( !type_checker.check() ){
                return 0;
            }
        }       
        return py_seq;
    }

    static void 
    construct( PyObject* py_seq, bpl::converter::rvalue_from_python_stage1_data* data){
        if( !convertible( py_seq ) ){
            bpl::throw_error_already_set();
        }

        typedef bpl::converter::rvalue_from_python_storage<colour_t> colour_storage_t;
        colour_storage_t* the_storage = reinterpret_cast<colour_storage_t*>( data );
        void* memory_chunk = the_storage->storage.bytes;
        colour_t* colour = new (memory_chunk) colour_t();
        data->convertible = memory_chunk;

        bpl::object seq = bpl::object( bpl::handle<>( bpl::borrowed( py_seq ) ) );        
        //Now I actually creates the object from the Python tuple
        bpl::object tmp = seq[0];
        colour->red = bpl::extract< float >( tmp );
        tmp = seq[1];
        colour->green = bpl::extract< float >( tmp );
        tmp = seq[2];
        colour->blue = bpl::extract< float >( tmp );

    }
    
};

void register_conversion(){
        
    bpl::converter::registry::push_back(  &from_tuple::convertible
                                        , &from_tuple::construct
                                        , bpl::type_id<colour_t>() );
}

}//namespace colour_conversion

BOOST_PYTHON_MODULE( from_conversion ){
    bpl::class_< colour_t >( "colour_t" )
        .def( bpl::init< bpl::optional< float, float, float > >( 
              ( bpl::arg("red_")=0.0, bpl::arg("green_")=0.0, bpl::arg("blue_")=0.0 ) ) )
        .def_readwrite( "red", &colour_t::red )
        .def_readwrite( "green", &colour_t::green )
        .def_readwrite( "blue", &colour_t::blue );
    colour_conversion::register_conversion();
    
    bpl::class_< image_t >( "image_t" )
        //naive aproach that will not work - plain Python assignment
        //.def_readwrite( "background", &image_t::background )
        //You should use properties to force the conversion
        .add_property( "background"
                       , bpl::make_getter( &image_t::background )
                       , bpl::make_setter( &image_t::background ) )
        .def( "test_background", &image_t::test_background );
    
    bpl::def("test_val_010", &::test_val_010);
    bpl::def("test_cref_000", &::test_cref_000);
    bpl::def("test_ref_111", &::test_ref_111);    
    bpl::def("test_ptr_101", &::test_ptr_101);
    bpl::def("test_cptr_110", &::test_cptr_110);
}
