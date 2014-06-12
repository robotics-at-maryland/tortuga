// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __declarations_order_bug_true_to_be_exported_hpp__
#define __declarations_order_bug_true_to_be_exported_hpp__


struct declarations_order_bug_true{

struct tester_t{
    tester_t() 
    {}
        
    const char* do_smth( bool ){
        return "do_smth( bool )";        
    }
    
    const char* do_smth( int ){
        return "do_smth( int )";
    }
    
    const char* operator+=(const char *){
        return "operator+=(const char *)";
    }
   
    const char* append(const char*){
        return "append(const char *)";
    }

    const char* append(const char*, int){
        return "append(const char*, int)";
    }

    const char* operator+=(const char){
        return "operator+=(const char)";
    }

    const char* append(const char){
        return "append(const char)";
    }
};
    
};


#endif//__declarations_order_bug_true_to_be_exported_hpp__

