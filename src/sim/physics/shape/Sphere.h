#ifndef _RAM_SIM_PHYSICS_SHAPE_H
#define _RAM_SIM_PHYSICS_SHAPE_H

#include "Shape.h"
#include "../../SimGL.h"

namespace ram {
    namespace sim {
        namespace physics {
            namespace shape {
                
                class Sphere : public Shape {
                public:
                    Sphere(float r) { setRadius(r); }
                    
                    void setRadius(float r)
                    {
                        radius = r;
                        volume = 4.0f / 3.0f * M_PI * r * r * r;
                        moment.x = moment.y = moment.z = 0.4f * r * r;
                    }
                    
                    float getRadius() const
                    { return radius; }
                    
                    virtual float getVolume() const
                    { return volume; }
                    
                    virtual const math::Vector3 getMoment() const
                    { return moment; }
                    
                    virtual void debugDraw() const
                    { glutWireSphere(radius, 12, 12); }
                    
                private:
                    float radius;
                    float volume;
                    math::Vector3 moment;
                };
                
            }
        }
    }
}

#endif
