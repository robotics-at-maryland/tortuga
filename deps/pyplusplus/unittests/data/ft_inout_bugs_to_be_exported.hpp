
namespace tests{
    
inline void set_flag( bool value, bool& flag ){
    flag = value;
}

struct base{
    virtual void set_flag( bool value, bool& flag ){
        flag = value;
    }    
};

struct inventor : base{
    virtual void set_flag( bool value, bool& flag ){
        flag = !value;
    }    
};

inline void call_set_flag( base& x, bool value, bool& flag ){
    x.set_flag( value, flag );
}

}
