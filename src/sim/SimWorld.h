#ifndef _RAM_SIM_SIMWORLD_H
#define _RAM_SIM_SIMWORLD_H

#include <IceUtil/Mutex.h>

#include <btBulletDynamicsCommon.h>
#include <GLDebugDrawer.h>

#include <list>

namespace ram {
    namespace sim {
        
        class SimWorld;
        
        class PhysicsUpdatable {
        protected:
            virtual void stepSimulation(SimWorld& world, btScalar timeStep) =0;
        };
        
        class RigidBody : PhysicsUpdatable {
            friend class SimWorld;
        protected:
            btDefaultMotionState motionState;
            btRigidBody body;
        public:
            RigidBody(btScalar mass, btCollisionShape* collisionShape, const btVector3& localInertia=btVector3(0,0,0))
            : motionState(), body(mass, &motionState, collisionShape, localInertia) {}
            virtual ~RigidBody() {}
            
            const btVector3& getLinearVelocity() const
            { return body.getLinearVelocity(); }
            
            const btVector3& getAngularVelocity() const
            { return body.getAngularVelocity(); }
            
            const btVector3& getCenterOfMassPosition() const
            { return body.getCenterOfMassPosition(); }
            
            const btTransform& getCenterOfMassTransform() const
            { return body.getCenterOfMassTransform(); }
            
            btQuaternion getOrientation() const
            { return body.getOrientation(); }
            
        protected:
            virtual void attachToWorld(btDynamicsWorld& btWorld)
            {
                btWorld.addRigidBody(&body);
            }
            virtual void detachFromWorld(btDynamicsWorld& btWorld)
            {
                btWorld.removeRigidBody(&body);
            }
        };
        
        class SimWorld {
        private:
            btAxisSweep3 broadphase;
            btDefaultCollisionConfiguration collisionConfiguration;
            btCollisionDispatcher dispatcher;
            btDiscreteDynamicsWorld btWorld;
            GLDebugDrawer gDebugDrawer;
            
            std::list<RigidBody*> bodies;
            
            IceUtil::Mutex physicsMutex;
        public:
            SimWorld()
            : broadphase(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 16834),
            collisionConfiguration(),
            dispatcher(&collisionConfiguration),
            btWorld(&dispatcher, &broadphase, NULL, &collisionConfiguration)
            {
                std::cerr << "Initializing physics." << std::endl;
                btWorld.setGravity(btVector3(0,0,-9.811));
            }
            
            ~SimWorld()
            {
                std::cerr << "Tearing down physics." << std::endl;
            }
            
            inline int	stepSimulation( btScalar timeStep,int maxSubSteps=1, btScalar fixedTimeStep=btScalar(1.)/btScalar(60.))
            {
                IceUtil::Mutex::Lock lock(physicsMutex);
                btWorld.setDebugDrawer(&gDebugDrawer);
                btWorld.getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
                for (std::list<RigidBody*>::iterator iter = bodies.begin();
                    iter != bodies.end() ; iter ++)
                    (*iter)->stepSimulation(*this, timeStep);
                return btWorld.stepSimulation(timeStep, maxSubSteps, fixedTimeStep);
            }
            
            inline btScalar getFluidDensity() const
            {
                return 1000; /* density of water in kg / m^3 */
            }
            
            inline btScalar getFluidLevel() const
            {
                return 10; /* water level, defined to be 0 */
            }
            
            inline btVector3 getGravity() const
            {
                return btWorld.getGravity();
            }
            
            inline void removeRigidBody(RigidBody* body)
            {
                IceUtil::Mutex::Lock lock(physicsMutex);
                std::cerr << "Removing a rigid body." << std::endl;
                bodies.remove(body);
                body->detachFromWorld(btWorld);
            }
            
            inline void addRigidBody(RigidBody* body)
            {
                IceUtil::Mutex::Lock lock(physicsMutex);
                std::cerr << "Adding a rigid body." << std::endl;
                bodies.push_back(body);
                body->attachToWorld(btWorld);
            }
            
            inline void debugDraw()
            {
                IceUtil::Mutex::Lock lock(physicsMutex);
                btWorld.debugDrawWorld();
            }
        };
        
        class BuoyantBody : public RigidBody {
        private:
            btScalar buoyantVolume;
            btVector3 centerOfBuoyancy;
            btScalar wetLinearDamping;
            btScalar dryLinearDamping;
            btScalar wetAngularDamping;
            btScalar dryAngularDamping;
        public:
            BuoyantBody(btScalar mass, btCollisionShape* collisionShape, const btVector3& localInertia=btVector3(0,0,0))
            : RigidBody(mass, collisionShape, localInertia),
            wetLinearDamping(0.8),
            dryLinearDamping(0),
            wetAngularDamping(0.8),
            dryAngularDamping(0) {}
            
            virtual ~BuoyantBody() {}
            
            virtual void setCenterOfBuoyancyPosition(const btVector3& v)
            { centerOfBuoyancy = v; }
            
            virtual const btVector3& getCenterOfBuoyancyPosition() const
            { return centerOfBuoyancy; }
            
            virtual void setBuoyantVolume(btScalar mBuoyantVolume)
            { buoyantVolume = mBuoyantVolume; }
            
            virtual btScalar getBuoyantVolume() const
            { return buoyantVolume; }
            
        protected:
            virtual void stepSimulation(SimWorld& world, btScalar timeStep)
            {
                const btVector3 cob = btTransform(body.getOrientation()) * getCenterOfBuoyancyPosition();
                const btVector3 cobAbsolute = cob + body.getCenterOfMassPosition();
                bool isSubmerged = cobAbsolute.dot(body.getGravity().normalized()) > -world.getFluidLevel();
                if (isSubmerged)
                {
                    body.applyForce(-world.getGravity() * world.getFluidDensity() * getBuoyantVolume(), cob);
                    body.setDamping(wetLinearDamping, wetAngularDamping);
                } else {
                    body.setDamping(dryLinearDamping, dryAngularDamping);
                }
            }
        };
        
    }
}

#endif
