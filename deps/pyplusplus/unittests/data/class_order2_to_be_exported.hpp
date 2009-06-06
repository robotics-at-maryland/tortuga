// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __class_order2_to_be_exported_hpp__
#define __class_order2_to_be_exported_hpp__

namespace class_order2{ 

struct S1;
struct S2;
    
struct S1{
    void do_smth( S2* s2=0 ){};
    //void do_smth( int i, S2* s2=NULL ){};
    void do_smth( double d, S2* s2=(S2*)(0) ){};
};

struct S2{
    void do_smth( S1* S1=0 ){};
    //void do_smth( int i, S1* S1=NULL ){};
    void do_smth( double d, S1* S1=(S1*)(0) ){};
};


}

#endif//__class_order2_to_be_exported_hpp__
