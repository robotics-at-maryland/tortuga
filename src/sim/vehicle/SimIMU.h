#ifndef _RAM_SIM_SIMIMU_H
#define _RAM_SIM_SIMIMU_H

#include <ram.h>
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"
#include "SimDevice.h"

namespace ram {
    namespace sim {
        class SimIMU : virtual public vehicle::IIMU, public SimDevice
        {
        private:
            SimVehicle& parent;
            btTransform localTransform;
        public:
            SimIMU(SimVehicle& mParent, std::string mName) : parent(mParent), SimDevice(mName), localTransform(btQuaternion(0, 0, 0, 1)) {}
            
            virtual transport::Vector3Ptr getLinearAcceleration(const ::Ice::Current&c)
            { return new math::Vector3; /* TODO */ }
            
            virtual transport::Vector3Ptr getAngularRate(const ::Ice::Current&c)
            {
                const btVector3& btVec = btTransform(parent.getCenterOfMassTransform().getRotation()) * parent.getAngularVelocity();
                return new math::Vector3(btVec.x(), btVec.y(), btVec.z());
            }
            
            virtual transport::QuaternionPtr getOrientation(const ::Ice::Current&c)
            {
                btQuaternion btQuat = (btTransform(parent.getCenterOfMassTransform()) * localTransform).getRotation();
                return new math::Quaternion(btQuat.x(), btQuat.y(), btQuat.z(), btQuat.w());
            }
            
            virtual void stepSimulation(SimWorld& world, btScalar timeStep)
            { /* TODO */ }
        };
    }
}

#endif
