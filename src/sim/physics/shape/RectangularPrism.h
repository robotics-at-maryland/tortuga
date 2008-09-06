#ifndef _RAM_SIM_PHYSICS_SHAPE_RECTANGULARPRISM_H
#define _RAM_SIM_PHYSICS_SHAPE_RECTANGULARPRISM_H

#include "Shape.h"

namespace ram {
    namespace sim {
        namespace physics {
            namespace shape {
                
                class RectangularPrism : public Shape {
                public:
                    /// Construct a cube (regular rectangular prism)
                    RectangularPrism(float sideLen) {
                        setDimensions(sideLen, sideLen, sideLen);
                    }
                    
                    /// Construct a general rectangular prism
                    RectangularPrism(float xLength, float yLength, float zLength) {
                        setDimensions(xLength, yLength, zLength);
                    }
                    
                    /// Set the dimensions of the rectangular prism
                    void setDimensions(float xLength, float yLength, float zLength) {
                        xlen = xLength;
                        ylen = yLength;
                        zlen = zLength;
                        volume = xlen * ylen * zlen;
                        moment.x = (ylen * ylen + zlen * zlen) / 12.0f;
                        moment.y = (xlen * xlen + zlen * zlen) / 12.0f;
                        moment.z = (xlen * xlen + ylen * ylen) / 12.0f;
                    }
                    
                    virtual void debugDraw() const {
                        glPushMatrix();
                        glScalef(xlen, ylen, zlen);
                        glutWireCube(1.0f);
                        glPopMatrix();
                    }
                    
                    virtual float getVolume() const
                    { return volume; }
                    
                    virtual const math::Vector3 getMoment() const
                    { return moment; }
                    
                private:
                    float xlen, ylen, zlen;
                    float volume;
                    math::Vector3 moment;
                };
                
            }
        }
    }
}

#endif
