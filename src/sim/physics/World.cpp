#include "World.h"
#include "Body.h"

#include "math/include/Vector3.h"

namespace ram {
    namespace sim {
        namespace physics {
            
            World::World() : gravity(9.811), fluidDensity(1000) {}
            
            World::~World() {}
            
            void World::step(float dt)
            {
                // TODO
            }
            
            float World::getGravity() const
            {
                return gravity;
            }
            
            void World::setGravity(float g)
            {
                gravity = g;
            }
            
            float World::getFluidDensity() const
            {
                return fluidDensity;
            }
            
            void World::setFluidDensity(float density)
            {
                fluidDensity = density;
            }
            
            void World::stepIntegrate(Body& body, float dt)
            {
                if (body.isStatic()) return;
                
                // Update linear coordinates
                body.addForceWorld(math::Vector3(0, 0, -body.mass * gravity));
                body.worldPosition += body.worldLinearVelocity * dt + 0.5f * dt * dt * body.inverseMass * body.worldForce;
                body.worldLinearVelocity += dt * body.inverseMass * body.worldForce;
                
                // Update angular coordiantes
                math::Vector3 worldAngularAcceleration = body.worldOrientation.Transform(body.inverseMomentOfInertia * body.getTorqueBody());
                body.worldOrientation = body.worldOrientation + 0.5f * math::Quaternion(dt * body.worldAngularVelocity + 0.5f * dt * dt * worldAngularAcceleration);
                body.worldOrientation.normalise();
                body.worldAngularVelocity += dt * worldAngularAcceleration;
                
                body.clearForceAndTorque();
            }
            
        }
    }
}
