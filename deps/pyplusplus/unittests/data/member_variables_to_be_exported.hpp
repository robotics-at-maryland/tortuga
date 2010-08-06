// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __member_variables_to_be_exported_hpp__
#define __member_variables_to_be_exported_hpp__
#include <memory>
#include <string>
#include <iostream>

namespace member_variables{

struct point{
    enum color{ red, green, blue };

    point()
    : prefered_color( blue )
      , x( -1 )
      , y( 2 )
    {++instance_count;}

    point( const point& other )
    : prefered_color( other.prefered_color )
      , x( other.x )
      , y( other.y )
    {}

    ~point()
    { --instance_count; }

    int x;
    int y;
    const color prefered_color;
    static int instance_count;
    static const color default_color;
};

struct bit_fields_t{
    bit_fields_t()
    : b(28){}
    unsigned int a : 1;
    unsigned int : 0;
    const unsigned int b : 11;
};

unsigned int get_a(const bit_fields_t& inst);
void set_a( bit_fields_t& inst, unsigned int new_value );
unsigned int get_b(const bit_fields_t& inst);

struct array_t{
    array_t(){
        for( int i = 0; i < 10; ++i ){
            ivars[i] = -i;
        }
    }

    struct variable_t{
        variable_t() : value(-9){}
        int value;
    };

    int get_ivars_item( int index ){
        return ivars[index];
    }

    const variable_t vars[3];
    int ivars[10];
    int ivars2[10];
};

namespace pointers{

struct data_t{
    data_t() : value( 201 ) {}
    int value;
    static char* reserved;
};

struct tree_node_t{

    data_t *data;
    tree_node_t *left;
    tree_node_t *right;
    const tree_node_t *parent;

    tree_node_t(const tree_node_t* parent=0)
    : data(0)
      , left( 0 )
      , right( 0 )
      , parent( parent )
    {}

    ~tree_node_t(){
        std::cout << "\n~tree_node_t";
    }
};

std::auto_ptr<tree_node_t> create_tree();

}

namespace reference{

enum EFruit{ apple, orange };

struct fundamental_t{
    fundamental_t( EFruit& fruit, const int& i )
    : m_fruit( fruit ), m_i( i )
    {}

    EFruit& m_fruit;
    const int& m_i;
};

struct A{};


struct B {
 B( A& a_ ): a( a_ ){}
 A& a;
};

struct C {
 C( A& a_ ): a( a_ ){}
 const A& a;
};

}

namespace statics{

struct mem_var_str_t{
    static std::string class_name;
    std::string identity(std::string x){ return x; }
};

}

namespace bugs{
struct allocator_ {
   void * (*alloc) (unsigned);
   void (*dispose) (void *p);
};

typedef struct allocator_ *allocator_t;

struct faulty {
   allocator_t allocator;
};

}

namespace ctypes{
    struct image_t{
        image_t(){
            data = new int[5];
            for(int i=0; i<5; i++){
                data[i] = i;
            }
        }
        int* data;

        static int* none_image;
    };
    
    class Andy{
    protected:
        Andy() : userData(NULL) {}
   
        virtual ~Andy()    {}

    public:
        void * userData;
    };

}

}
#endif//__member_variables_to_be_exported_hpp__
