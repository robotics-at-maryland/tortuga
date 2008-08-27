#ifndef _RAM_SIM_PHYSICS_BODY_H
#define _RAM_SIM_PHYSICS_BODY_H

#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"
#include "math/include/Matrix3.h"

namespace ram {
    namespace sim {
        namespace physics {
            class World;
            class Shape;
            
            class Body {
                friend class World;
            public:
                Body();
                ~Body();
                
                /// Get the body's mass
                float getMass() const;
                
                /// Set the body's mass.
                /// Setting an infinite mass will make the body static.
                void setMass(float);
                
                /// Make the body static by setting its mass to infinity.
                void makeStatic();
                
                /// Determine whether the body is static or dynamic.
                /// @return true  if the body is static (has infinite mass)
                /// @return false if the body is dynamic (has finite mass)
                bool isStatic() const;
                
                /// Add a force in body coordinates.
                void addForceBody(const math::Vector3& force);
                
                /// Add a force in world coordinates.
                void addForceWorld(const math::Vector3& force);
                
                /// Add a torque in body coordinates.
                void addTorqueBody(const math::Vector3& torque);
                
                /// Add a torque in world coordinates.
                void addTorqueWorld(const math::Vector3& torque);
                
                /// Add a force at an offset in body coordinates.
                void addRelForceBody(const math::Vector3& force,
                                     const math::Vector3& position);
                
                /// Add a force at an offset in world coordinates.
                void addRelForceWorld(const math::Vector3& force,
                                      const math::Vector3& position);
                
                const math::Vector3& getForceWorld() const;
                const math::Vector3 getForceBody() const;
                const math::Vector3& getTorqueWorld() const;
                const math::Vector3 getTorqueBody() const;
                
            private:
                // Physical composition
                float mass;
                float inverseMass;
                bool hasInfiniteMass;
                float volume;
                math::Matrix3 bodyMomentOfInertia;
                math::Vector3 collisionShapeOffset;
                Shape collisionShape;                
                
                // Kinematic coordinates
                /// Quaternion that transforms body frame coordinates to world coordinates
                math::Quaternion worldOrientation;
                math::Vector3 worldPosition;
                math::Vector3 worldLinearVelocity;
                math::Vector3 worldAngularVelocity;
                
                // Forces and torques
                math::Vector3 worldForce;
                math::Vector3 worldTorque;
                
                // Helper routines
                void clearForceAndTorque();
            };
        }
    }
}

#endif
