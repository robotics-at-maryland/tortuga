#include "custom_smart_ptr.h"

namespace controllers{

struct controller_i{
public:
    virtual ~controller_i() { }
    virtual int get_value(void) const = 0;
};

typedef smart_ptrs::my_smart_ptr_t<controller_i> controller_ptr_i;

struct multiply_x_t : controller_i{
    multiply_x_t( int value )
    : m_value( value )
    {}

    virtual int get_value(void) const{
        return  m_value * get_multiply_value();
    }

    virtual int get_multiply_value() const{
        return 1;
    }

private:
    int m_value;
};

typedef smart_ptrs::my_smart_ptr_t<multiply_x_t> multiply_x_ptr_t;

struct add_x_t : controller_i{
    add_x_t( int value )
    : m_value( value )
    {}

    virtual int get_value(void) const{
        return  m_value + get_add_value();
    }

    virtual int get_add_value() const{
        return 0;
    }

private:
    int m_value;
};


struct add_x_ptr_t : public smart_ptrs::my_smart_ptr_t< add_x_t >{

    explicit add_x_ptr_t(add_x_t* rep)
    : smart_ptrs::my_smart_ptr_t<add_x_t>(rep)
    {}

    add_x_ptr_t(const add_x_ptr_t& r)
    : smart_ptrs::my_smart_ptr_t<add_x_t>(r)
    {}

    //added by me( Roman ), to allow implicit conversion between add_x_ptr_t and add_x_ptr_t
    //operator smart_ptrs::my_smart_ptr_t<resource_t>() const {
    //    return smart_ptrs::my_smart_ptr_t<resource_t>( *this );
    //}

    /// Operator used to convert a add_x_ptr_t to a add_x_ptr_t
    add_x_ptr_t& operator=(const add_x_ptr_t& r){

        if( pRep == static_cast<add_x_t*>(r.getPointer()) ){
            return *this;
        }

        release();

        pRep = static_cast<add_x_t*>(r.getPointer());
        pUseCount = r.useCountPointer();

        if (pUseCount){
            ++(*pUseCount);
        }

        return *this;
    }
};

add_x_ptr_t create_sptr_add_x(){
    return add_x_ptr_t( new add_x_t(12) );
}

}



//not supported
//add_x_ptr_t create_shared_resource(){
//    return add_x_ptr_t( new add_x_t() );
//}

inline int
ref_get_value( controllers::controller_ptr_i& a ){
    return a->get_value();
}

//inline int
//val_get_value( smart_ptrs::my_smart_ptr_t< controllers::add_x_t > a ){
//    return a->get_value();
//}


inline int
val_get_value( controllers::controller_ptr_i a ){
    return a->get_value();
}

inline int
const_ref_get_value( const controllers::controller_ptr_i& a ){
    return a->get_value();
}


