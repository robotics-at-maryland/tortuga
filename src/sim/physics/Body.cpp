#include "Body.h"

#include <limits>
#include <assert.h>

namespace ram {
    namespace sim {
        namespace physics {
            
            Body::Body()
            {
                makeStatic();
            }
            
            Body::~Body() {}
            
            float Body::getMass() const
            {
                return mass;
            }
            
            void Body::setMass(float mMass)
            {
                assert(mMass >= 0.0);
                
                if (mMass == std::numeric_limits<float>::infinity() || mMass == std::numeric_limits<float>::max()) {
                    makeStatic();
                } else {
                    hasInfiniteMass = false;
                    mass = mMass;
                    inverseMass = 1.0 / mass;
                }
            }
            
            void Body::makeStatic()
            {
                inverseMass = 0.0;
                mass = std::numeric_limits<float>::infinity();
                hasInfiniteMass = true;
            }
            
            bool Body::isStatic() const
            {
                return hasInfiniteMass;
            }
            
            void Body::addForceBody(const math::Vector3& force)
            {
                if (isStatic()) return;
                addForceWorld(worldOrientation.Transform(force));
            }
            
            void Body::addForceWorld(const math::Vector3& force)
            {
                if (isStatic()) return;
                worldForce += force;
            }
            
            void Body::addTorqueBody(const math::Vector3& torque)
            {
                if (isStatic()) return;
                addTorqueWorld(worldOrientation.Transform(torque));
            }
            
            void Body::addTorqueWorld(const math::Vector3& torque)
            {
                if (isStatic()) return;
                worldTorque += torque;
            }
            
            void Body::addRelForceBody(const math::Vector3& force,
                                       const math::Vector3& position)
            {
                if (isStatic()) return;
                addForceBody(force);
                addTorqueBody(position.crossProduct(force));
            }
            
            void Body::addRelForceWorld(const math::Vector3& force,
                                        const math::Vector3& position)
            {
                if (isStatic()) return;
                addForceWorld(force);
                addTorqueWorld((position - worldPosition).crossProduct(force));
            }
            
            const math::Vector3& Body::getForceWorld() const
            {
                return worldForce;
            }
            
            const math::Vector3 Body::getForceBody() const
            {
                return worldOrientation.UnitInverse().Transform(worldTorque);
            }
            
            const math::Vector3& Body::getTorqueWorld() const
            {
                return worldTorque;
            }
            
            const math::Vector3 Body::getTorqueBody() const
            {
                return worldOrientation.UnitInverse().Transform(worldTorque);
            }
            
            void Body::clearForceAndTorque()
            {
                worldForce = 0;
                worldTorque = 0;
            }
            
        }
    }
}
