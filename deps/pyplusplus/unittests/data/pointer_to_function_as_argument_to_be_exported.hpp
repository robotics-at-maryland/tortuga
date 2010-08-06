// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __pointer_to_function_as_argument_to_be_exported_hpp_
#define __pointer_to_function_as_argument_to_be_exported_hpp_

namespace fixme{

struct dummy{};

typedef bool ( *do_smth )( const dummy& );

struct other_dummy{

    //this function should not be exported at all
    void do_nothing( do_smth new_do_smth );
    
    //this function should not be exported at all to
    do_smth current_do_nothing();      
};

}
#endif //__pointer_to_function_as_argument_to_be_exported_hpp_

