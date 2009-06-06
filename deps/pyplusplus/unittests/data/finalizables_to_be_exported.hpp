// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __finalizables_to_be_exported_hpp__
#define __finalizables_to_be_exported_hpp__

namespace finalizables{

struct function_t{
    virtual int year(){ return 2004; } 
};

struct whole_class_t{
    virtual int month(){ return 12; } 
    virtual int day(){ return 16; } 
protected:
    void do_smth(){};
};

}

#endif//__finalizables_to_be_exported_hpp__
