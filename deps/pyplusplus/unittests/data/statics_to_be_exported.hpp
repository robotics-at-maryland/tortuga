// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __statics_to_be_exported_hpp__
#define __statics_to_be_exported_hpp__

namespace statics{

struct data{

    data(){
        instances += 1;
    }
    
    ~data(){
        instances -= 1;
    }

public:

    static int instances_count(){
        return instances;
    }

private:
    static int instances;
};

inline int data_instances_count(){
    return data::instances_count();
}

}//statics 

#endif//__statics_to_be_exported_hpp__

