// Copyright 2004 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __function_transformations_to_be_exported_hpp__
#define __function_transformations_to_be_exported_hpp__

#include <cmath>
#include <string>
#include <iostream>

namespace ft2{
    
void hello_world( std::string& hw ){
    hw = "hello world!";
}
    
//used to check output transformer
struct calculator_t{
    calculator_t(){
    }

    virtual ~calculator_t(){
    }
    
    int assign_0_1_2( int& one, int& two ){
        one = 1;
        two = 2;
        return 0;
    }
    
    void assign_1_2( int& one, int& two ){
        assign_0_1_2( one, two );
    }
    
    calculator_t* clone_and_assign_5( int& five ){
        five = 5;
        return new calculator_t();
    }
};

//used to check input\inout transformers
struct window_t{
    window_t()
    : height( 0 )
      , width( 0 )
    {}
    
    void resize( int& h, int& w ){
        height = h;
        width = w;
    }
    
    int resize_in_out( int& h, int& w){
        height *= h; 
        h = height;
        width *= w;
        w = width;
        return h*w;
    }
    
    int height;
    int width;
};

struct point3d_t{
    
    point3d_t()
    : x( 0 ), y(0), z(0)
    {}
        
    int initialize( int v[3] ){
        x = v[0];
        y = v[1];
        z = v[2];
        return x*y*z;
    }
    
    void position( int v[3] ){
        v[0] = x;
        v[1] = y;
        v[2] = z;        
    }        
    
    static void distance( const point3d_t& pt, double& dist ){
        dist = sqrt( double( pt.x*pt.x + pt.y*pt.y + pt.z*pt.z ) );
    }
    
    int x, y, z;
};
    
}

namespace ft{

struct image_t{

    image_t( unsigned int h, unsigned int w )
    : m_height( h )
      , m_width( w )
    {}

    // Made the method 'virtual' for now because func transformers
    // are currently only taken into account on virtual functions.
    virtual void get_size( unsigned int& h, unsigned int& w ){
        h = m_height;
        w = m_width;
    }

    // Return only one value
    virtual void get_one_value(unsigned int& h) {
        h = m_height;
    }

    // Like get_size() but with a return type and an additional argument
    // that has to be kept in the signature
    virtual int get_size2( unsigned int& h, unsigned int& w, int noref=0 ){
        h = m_height;
        w = m_width;
	return noref;
    }

    // A method with an input argument
    virtual int input_arg(int& in){
      return in;
    }

    // A method taking an input array of fixed size
    virtual int fixed_input_array(int v[3]) {
      return v[0]+v[1]+v[2];
    }

    // A method with a output array of fixed size
    virtual void fixed_output_array(int v[3]) {
      v[0] = 1;
      v[1] = 2;
      v[2] = 3;
    }

    unsigned int m_width;
    unsigned int m_height;

};

// Provide an instance created in C++ (i.e. this is not a wrapper instance)
image_t cpp_instance(12,13);
image_t& get_cpp_instance() {
  return cpp_instance;
}

inline void
get_image_size( image_t& img, unsigned int& h, unsigned int& w ){
    img.get_size( h, w );
}

// This is used for calling img.get_one_value() on an instance passed
// in by Python.
unsigned int get_image_one_value( image_t& img ) {
  unsigned int v;
  img.get_one_value(v);
  return v;
}

// This is used for calling img.fixed_output_array() on an instance passed
// in by Python.
int image_fixed_output_array( image_t& img) {
  int v[3];
  img.fixed_output_array(v);
  return v[0]+v[1]+v[2];
}

//////////////////////////////////////////////////////////////////////

// A class without any virtual members
struct no_virtual_members_t
{
  bool member(int& v) { v=17; return true; }
};

struct ft_private_destructor_t{
	static void get_value( int& x ){ x = 21; }
private:
	~ft_private_destructor_t(){}
};

struct bug_render_target_t{
    
    static float get_static_statistics( bug_render_target_t& inst ){
        float x,y;
        inst.get_statistics( x, y );
        return x+y;
    }
    
	virtual void get_statistics( float& x, float& y ) const{
		x = 1.1;;
		y = 1.2;
	}
	
	struct frame_stats_t{
		float x, y;
	};
	
	virtual frame_stats_t get_statistics(){
		frame_stats_t fs;
		fs.x = 1.1;
		fs.y = 1.2;
        return fs;
	}
};

struct modify_type_tester_t{
    int do_nothing( int& v ){
        return v;
    }
    
    modify_type_tester_t* clone(int& c){
        return new modify_type_tester_t();
    }
    
};

struct input_c_buffer_tester_t{
	std::string write( char* buffer, int dummy, int size ) const {
		return std::string( buffer, size );
	}
	
	static std::string write_s( int dummy, char* buffer, int size ){
		return std::string( buffer, size );
	}

};

struct transfer_ownership_tester_t{
    struct resources_t{
        resources_t(){
            std::cout << "created";
        }
        ~resources_t(){
            std::cout << "destroyed";
        }
    };
    void tester(resources_t* r){
        delete r;
    }
};

}

#endif//__function_transformations_to_be_exported_hpp__
