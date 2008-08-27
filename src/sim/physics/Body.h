#ifndef _RAM_SIM_PHYSICS_BODY_H
#define _RAM_SIM_PHYSICS_BODY_H

namespace ram {
    namespace sim {
        namespace physics {
            class Body {
            public:
                Body();
                ~Body();
                
                /// Get the body's mass
                float getMass() const;
                
                /// Set the body's mass
                void setMass(float);
                
                /// Add a force in body coordinates.
                void addForceBody(const math::Vector3& force);
                
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
                
            private:
                float mass;
                float inverseMass;
                float volume;
                math::Quaternion worldOrientation;
                math::Vector3 worldPosition;
                math::Vector3 worldLinearVelocity;
                math::Vector3 worldAngularVelocity;
                math::Vector3 worldForce;
                math::Vector3 worldTorque;
                math::Matrix3 bodyMomentOfInertia;
                math::Vector3 collisionShapeOffset;
                Shape collisionShape;                
            };
        }
    }
}

#endif
