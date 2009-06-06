#ifndef __properties_h__
#define __properties_h__

namespace geometry{
    
struct point_t{

    explicit point_t(int x=0, int y=0)
    : m_x(x), m_y(0)
    {}
    
    int get_x() const { return m_x; }
    void set_x(int x) { m_x = x; }

    int get_y() const { return m_y; }
    void set_y(int y) { m_y = y; }
    
private:
    int m_x;
    int m_y;
};

inline int
extract_x( const point_t& pt ){ return pt.get_x(); }

inline int
extract_y( const point_t& pt ){ return pt.get_y(); }

}

#endif//__properties_h__

