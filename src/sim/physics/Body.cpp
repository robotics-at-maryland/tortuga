#include "Body.h"

namespace ram {
    namespace sim {
        namespace physics {
            
            Body::Body() {}
            
            Body::~Body() {}
            
            float Body::getMass() const {
                return mass;
            }
            
            void Body::setMass(float mMass) {
                inverseMass = 1.0 / mass;
            }
            
            void Body::addForceBody(const math::Vector3& force) {
            }
            
            /// Add a force in world coordinates.
            void addForceWorld(const math::Vector3& force);
            
            /// Add a torque in body coordinates.
            void addTorqueBody(const math::Vector3& force);
            
            /// Add a torque in world coordinates.
            void addTorqueWorld(const math::Vector3& force);
            
            /// Add a force at an offset in body coordinates.
            void addRelForceBody(const math::Vector3& force,
                                 const math::Vector3& position);
            
            /// Add a force at an offset in world coordinates.
            void addRelForceWorld(const math::Vector3& force,
                                  const math::Vector3& position);
            
        }
    }
}
