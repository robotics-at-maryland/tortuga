// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __classes_to_be_exported_hpp__
#define __classes_to_be_exported_hpp__

namespace classes{

namespace fundamentals{

struct fundamental1{};

struct fundamental2{};

}

namespace hierarchical{

struct fruit{};

struct apple : public fruit{};

}

namespace noncopyables{

class noncopyable1{
public:
    noncopyable1(){}
private:
    noncopyable1( const noncopyable1& );
};

}

namespace abstracts{

class abstract{
public:
    abstract(){}
    abstract(int){}
    abstract(int, double, const abstract&){}
public:

    virtual int pure_virtual(const abstract& ) const = 0;

    virtual bool overloaded_virtual(){ return true;}
    virtual bool overloaded_virtual(int){ return true;}

    virtual int overloaded_pure_virtual(int) const = 0;
    virtual void overloaded_pure_virtual(double) const = 0;

    virtual int some_virtual(){ return 1; }
};

}

namespace constructors{

struct constructor1{
    constructor1(){}
    constructor1( const constructor1& ){}
    constructor1(int x, int y){}
    constructor1(int y, const constructor1& x ){}

    constructor1( const double ){}
    struct internal_data{};
    constructor1( const internal_data ){}
};

}

namespace scope_based{
struct scope_based_exposer{
    enum EColor{ red, green, blue };
    scope_based_exposer(EColor c=blue){}
};
}

namespace protected_static{
class protected_static_t{
protected:
    static int identity(int x){ return x; }

    int invert_sign(int x){ return -1*x; }
private:
    int i;
};
};

namespace non_public_hierarchy{

struct protected_base{};
struct protected_derived : protected protected_base{};

}

union Word
{
       short word;
       struct
       {
               char low, high;
       };
};

}//classes

namespace pyplusplus{ namespace aliases{

typedef classes::hierarchical::apple the_tastest_fruit;

typedef classes::protected_static::protected_static_t PROTECTED_STATIC_T_1;
typedef classes::protected_static::protected_static_t PROTECTED_STATIC_T_2;

}}

#endif//__classes_to_be_exported_hpp__
