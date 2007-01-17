#include "boost/python.hpp"
#include "tuple_conversion.hpp"
#include "boost/tuple/tuple_comparison.hpp"

//This example is very similar to one found in Boost.Python FAQs:
//How can I automatically convert my custom string type to and from a Python string?
//http://www.boost.org/libs/python/doc/v2/faq.html#custom_string
//I will introduce another example with detailed explanation. 

//Simple class, which describes a record in 3D. I don't want to export the class.
//Instead of this I want Python user pass tuple as argument to all functions
//that expects record_t class instance and Boost.Python will automaticly handle 
//the conversion. Same conversion should be applied on functions, which returns
//record_t class instance

typedef boost::tuple< int, int, int > record_t;

record_t record_ret_val_000() { 
    return record_t(0,0,0); 
}

record_t record_ret_val_101() { 
    return record_t(1,0,1); 
}

record_t& record_ret_ref_010(){
    static record_t pt( 0,1,0 );
    return pt;
}

record_t* record_ret_ptr_110(){
    static record_t pt( 1,1,0 );
    return &pt;
}

bool test_record_val_000( record_t pt ){
    return pt == record_t( 0,0,0 );
}

bool test_record_cref_010( const record_t& pt ){
    return pt == record_t( 0,1,0 );
}

bool test_record_ref_110( record_t& pt ){
    return pt == record_t( 1,1,0 );
}

bool test_record_ptr_101( record_t* pt ){
    return pt && *pt == record_t( 1,0,1 );
}


namespace bpl = boost::python;

BOOST_PYTHON_MODULE( auto_conversion ){
    bpl::register_tuple< record_t >();
    
    bpl::def("record_ret_val_000", &::record_ret_val_000);
    bpl::def("record_ret_val_101", &::record_ret_val_101);
    bpl::def("record_ret_ref_010"
             , &::record_ret_ref_010
             , bpl::return_value_policy<bpl:: copy_non_const_reference>() );
    bpl::def( "record_ret_ptr_110"
              , &::record_ret_ptr_110
              , bpl::return_value_policy<bpl::return_by_value>() );
    bpl::def("test_record_val_000", &::test_record_val_000);
    bpl::def("test_record_cref_010", &::test_record_cref_010);
    bpl::def("test_record_ref_110", &::test_record_ref_110);
    bpl::def("test_record_ptr_101", &::test_record_ptr_101);
}

