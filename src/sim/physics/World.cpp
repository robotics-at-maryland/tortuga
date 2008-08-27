#include "World.h"
#include "Body.h"

#include "math/include/Vector3.h"

namespace ram {
    namespace sim {
        namespace physics {
            
            World::World() : gravity(9.811), fluidDensity(1000) {}
            
            World::~World() {}
            
            void step(float dt)
            {
                // TODO
            }
            
            float getGravity() const
            {
                return gravity;
            }
            
            void setGravity(float g)
            {
                gravity = g;
            }
            
            float getFluidDensity() const
            {
                return fluidDensity;
            }
            
            void setFluidDensity(float density)
            {
                fluidDensity = density;
            }
            
            void stepIntegrate(Body& body, float dt)
            {
                if (body.isStatic()) return;
                
                // Update linear coordinates
                body.addForceWorld(math::Vector3(0, 0, -body.mass * gravity));
                body.worldPosition += body.worldVelocity * dt + 0.5 * dt * dt * body.inverseMass * body.worldForce;
                body.worldVelocity += dt * body.inverseMass * body.worldForce;
                
                // Update angular coordiantes
                Vector3 worldAngularAcceleration = body.worldOrientation * (body.inverseMomentOfInertia * body.getTorqueBody()) * body.worldOrientation.conj();
                body.worldOrientation += 0.5 * (dt * body.worldAngularVelocity + 0.5 * dt * dt * worldAngularAcceleration);
                body.worldOrientation.normalize();
                body.worldAngularVelocity += dt * worldAngularAcceleration;
                
                body.clearForceAndTorque();
            }
            
        }
    }
}
