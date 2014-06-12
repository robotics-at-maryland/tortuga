// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __member_functions_to_be_exported_hpp__
#define __member_functions_to_be_exported_hpp__

#include <string>

namespace member_functions{

struct protected_mf_t{
protected:
    static int x(){ return 1;}
};

struct protected_base_t{
protected:
    int regular( int x ){ return x; }
    int regular_overloaded( int x ){ return x * x; }
    int regular_overloaded( int x, int y ){ return x * y; }

    int regular_const( int x ) const { return x; }
    int regular_const_overloaded( int x ) const { return x * x; }
    int regular_const_overloaded( int x, int y) const { return x * y; }

//virtual

    virtual int virtual_( int x ){ return x; }
    virtual int virtual_overloaded( int x ){ return x * x; }
    virtual int virtual_overloaded( int x, int y ){ return x * y; }

    virtual int virtual_const( int x ) const { return x; }
    virtual int virtual_const_overloaded( int x ) const { return x * x; }
    virtual int virtual_const_overloaded( int x, int y ) const { return x * y; }

//pure virtual
    virtual int pure_virtual( int x ) = 0;
    virtual int pure_virtual_overloaded( int x ) = 0;
    virtual int pure_virtual_overloaded( int x, int y ) = 0;

    virtual int pure_virtual_const( int x ) const =  0;
    virtual int pure_virtual_const_overloaded( int x ) const = 0;
    virtual int pure_virtual_const_overloaded( int x, int y ) const = 0;
};

struct protected_public_derived_t : public protected_base_t{

    virtual int pure_virtual( int x ) { return x;}
    virtual int pure_virtual_overloaded( int x ) { return x * x; }
    virtual int pure_virtual_overloaded( int x, int y ){ return x * y; }

    virtual int pure_virtual_const( int x ) const { return x; }
    virtual int pure_virtual_const_overloaded( int x ) const { return x * x; }
    virtual int pure_virtual_const_overloaded( int x, int y ) const { return x * y; }
};

struct protected_protected_derived_t : public protected_base_t{

    virtual int pure_virtual( int x ) { return x;}
    virtual int pure_virtual_overloaded( int x ) { return x * x; }
    virtual int pure_virtual_overloaded( int x, int y ){ return x * y; }

    virtual int pure_virtual_const( int x ) const { return x; }
    virtual int pure_virtual_const_overloaded( int x ) const { return x * x; }
    virtual int pure_virtual_const_overloaded( int x, int y ) const { return x * y; }
};


struct public_base_t{
public:
    int regular( int x ){ return x; }
    int regular_overloaded( int x ){ return x * x; }
    int regular_overloaded( int x, int y ){ return x * y; }

    int regular_const( int x ) const { return x; }
    int regular_const_overloaded( int x ) const { return x * x; }
    int regular_const_overloaded( int x, int y) const { return x * y; }

//virtual

    virtual int virtual_( int x ){ return x; }
    virtual int virtual_overloaded( int x ){ return x * x; }
    virtual int virtual_overloaded( int x, int y ){ return x * y; }

    virtual int virtual_const( int x ) const { return x; }
    virtual int virtual_const_overloaded( int x ) const { return x * x; }
    virtual int virtual_const_overloaded( int x, int y ) const { return x * y; }

//pure virtual
    virtual int pure_virtual( int x ) = 0;
    virtual int pure_virtual_overloaded( int x ) = 0;
    virtual int pure_virtual_overloaded( int x, int y ) = 0;

    virtual int pure_virtual_const( int x ) const =  0;
    virtual int pure_virtual_const_overloaded( int x ) const = 0;
    virtual int pure_virtual_const_overloaded( int x, int y ) const = 0;
};

struct public_derived_t : public public_base_t{

    virtual int pure_virtual( int x ) { return x;}
    virtual int pure_virtual_overloaded( int x ) { return x * x; }
    virtual int pure_virtual_overloaded( int x, int y ){ return x * y; }

    virtual int pure_virtual_const( int x ) const { return x; }
    virtual int pure_virtual_const_overloaded( int x ) const { return x * x; }
    virtual int pure_virtual_const_overloaded( int x, int y ) const { return x * y; }
};


struct private_base_t{
    void remove_gcc_warning(){}
private:
    int regular( int x ){ return x; }
    int regular_overloaded( int x ){ return x * x; }
    int regular_overloaded( int x, int y ){ return x * y; }

    int regular_const( int x ) const { return x; }
    int regular_const_overloaded( int x ) const { return x * x; }
    int regular_const_overloaded( int x, int y) const { return x * y; }

//virtual

    virtual int virtual_( int x ){ return x; }
    virtual int virtual_overloaded( int x ){ return x * x; }
    virtual int virtual_overloaded( int x, int y ){ return x * y; }

    virtual int virtual_const( int x ) const { return x; }
    virtual int virtual_const_overloaded( int x ) const { return x * x; }
    virtual int virtual_const_overloaded( int x, int y ) const { return x * y; }

//pure virtual
    virtual int pure_virtual( int x ) = 0;
    virtual int pure_virtual_overloaded( int x ) = 0;
    virtual int pure_virtual_overloaded( int x, int y ) = 0;

    virtual int pure_virtual_const( int x ) const =  0;
    virtual int pure_virtual_const_overloaded( int x ) const = 0;
    virtual int pure_virtual_const_overloaded( int x, int y ) const = 0;
};

struct private_derived_t : public private_base_t{

    virtual int pure_virtual( int x ) { return x;}
    virtual int pure_virtual_overloaded( int x ) { return x * x; }
    virtual int pure_virtual_overloaded( int x, int y ){ return x * y; }

    virtual int pure_virtual_const( int x ) const { return x; }
    virtual int pure_virtual_const_overloaded( int x ) const { return x * x; }
    virtual int pure_virtual_const_overloaded( int x, int y ) const { return x * y; }
};

struct callable_t{
    int operator()( int i, int j ) const { return i + j; }
    int operator()( int i, int j, int k ) const { return i + j + k; }
};

struct immutable_by_ref_t{
    virtual std::string identity( std::string const & x ) = 0;

    static std::string
    call_identity( immutable_by_ref_t& imm, std::string const & x ){
        return imm.identity( x );
    }
};


struct mem_fun_environment_t{
    template< class T>
    T get_value(const std::string& x){ return T(); }
};

inline void instantiate_mem_fun_environment(){
    mem_fun_environment_t env;
    int i = env.get_value<int>("");
}

struct mem_fun_constness_t{
    static int do_smth( int, const int ){ return 0; }
    void do_smth_mem( int, const int ){}
};
}

#endif//__member_functions_to_be_exported_hpp__

