#ifndef shared_ptr_utils_h_F2700FFE_4D31_4948_B761_2CB29A747490
#define shared_ptr_utils_h_F2700FFE_4D31_4948_B761_2CB29A747490

#include "boost/python.hpp"
#include "Ogre.h"

template<class T>
inline T * get_pointer(Ogre::SharedPtr<T> const& p){
    return p.get();
}


namespace boost{ namespace python{

    using get_pointer;
    
    template <class T>
    struct pointee< Ogre::SharedPtr<T> >{
        typedef T type;
    };

} }


#endif//shared_ptr_utils_h_F2700FFE_4D31_4948_B761_2CB29A747490
