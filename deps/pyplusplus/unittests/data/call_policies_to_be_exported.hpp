// Copyright 2004 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __call_policies_to_be_exported_hpp__
#define __call_policies_to_be_exported_hpp__

namespace call_policies{

struct dummy{
    int id() { return int( this ); }
};

namespace return_arg{

inline void return_second_arg( int, int x, int )
{ ; }

}

namespace return_self{

inline const dummy& return_self( const dummy& x , int y )
{ return x; }

}

namespace return_value_policy{

inline const dummy& copy_const_reference( const dummy& x)
{ return x; }

struct container{
    int operator[]( int i ) const { return i; }
};

}

namespace void_ptr{

inline void* my_address(){
    return (void*)( &my_address );
}

inline bool compare( void* ptr ){
    return ptr == my_address();
}

}

namespace opaque{
    struct impl_details_t{};

    inline impl_details_t* get_impl_details(){
        return (impl_details_t*)( 0x11223344 );
    }

    typedef struct opaque_ *opaque_pointer;
    opaque_pointer get_opaque(){ return 0; }

}

namespace fundamental_ptr{

inline float* get_fundamental_ptr_value(){
    static float x = 0.5;
    return &x;
}

inline float* get_fundamental_ptr_value_null(){
    return (float*)(0);
}

}

struct arrays{
    int* create_arr_3(){
        int* arr3 = new int[3];
        for( int i = 0 ; i < 3; ++i ){
            arr3[i] = i;
        }
        return arr3;
    }
};

}

#endif//__call_policies_to_be_exported_hpp__
