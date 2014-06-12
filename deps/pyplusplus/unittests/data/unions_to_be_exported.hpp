namespace unions{

struct data_t{
    union actual_data_t{
        int i;
        double d;
    };
    actual_data_t data;
    
    double get_d() const{ return data.d; }
    int get_i() const{ return data.i; }
    
    void set_d( double d ){ data.d = d; }
    void set_i( int i ){ data.i = i; }
};

}

namespace anonymous_unions{

struct data2_t{
    union{
        int i;
        double d;
    };
    
    double get_d() const{ return d; }
    int get_i() const{ return i; }
    
    void set_d( double _d ){ d = _d; }
    void set_i( int _i ){ i = _i; }
    
};
    
}
