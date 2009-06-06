// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __noncopyable_to_be_exported_hpp__
#define __noncopyable_to_be_exported_hpp__

#include "boost/noncopyable.hpp"

namespace noncopyables{ 

class a_t{
public:

    static char get_a(){ return 'a'; }

private:
    a_t(){};
    ~a_t(){};
};

class b_t{
    ~b_t(){}
public:
   
    static char get_b(){ return 'b'; }

};

class c_t : public boost::noncopyable{
public:  
    static char get_c(){ return 'c'; }

};

class d_t{  
private:
    d_t( const d_t& );    
public:  
    d_t(){}
    ~d_t(){}
    static char get_d(){ return 'd'; }

};

class dd_t : public d_t{
public:
    dd_t(){}
    ~dd_t(){}
    static char get_dd(){ return 'D'; }        
};

struct e_t{
    virtual void do_smth() = 0;
private:
    c_t c;    
};

struct f_t{
    f_t() : i(0){}
    virtual void do_smth() = 0;
private:
    const int i;    
};

struct g_t{    
    enum E{e};
    g_t() : e_(e){}
    virtual void do_smth() = 0;
private:
    const E e_;    
};


}

#endif//__noncopyable_to_be_exported_hpp__