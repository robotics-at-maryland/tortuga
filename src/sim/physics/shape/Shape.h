#ifndef _RAM_SIM_PHYSICS_SHAPE_SHAPE_H
#define _RAM_SIM_PHYSICS_SHAPE_SHAPE_H

#include "math/include/Vector3.h"

/**
 * A Shape is a volumetric primitive.
 * To simplify calculations, all Shapes must be stored in such a form that
 * the centroid is at the origin, with the principal axes of rotation
 * on the x, y, and z axes, so that the moment of inertia tensor is
 * a diagonal matrix.
 */
namespace ram {
    namespace sim {
        namespace physics {
            namespace shape {
                class Shape {
                public:
                    /// Draw the shape in wireframe at the origin.
                    virtual void debugDraw() const { /* do nothing */ }
                    /// Return the volume of the shape.
                    virtual double getVolume() { return 0; }
                    /// Return the principal moments of inertia of the body,
                    /// assuming unit mass.  (Multiply the output of this method
                    /// to get the principal moments of inertia.)
                    virtual const math::Vector3 getMoment() const { return math::Vector3(); }
                };
            }
        }
    }
}

#endif
