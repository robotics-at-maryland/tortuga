#include "Camera.h"
#include "SimGL.h"

namespace ram {
    namespace sim {
        
        Camera::Camera() : yaw(0), pitch(0), roll(0), dollyDist(0) {} //transform(btQuaternion(0, 0, 0, 1)) {}
        
        void Camera::tumbleYaw(float deg)
        {
            yaw += deg;
            //transform *= btTransform(btQuaternion(btVector3(0, 0, 1), deg));
        }

        void Camera::tumblePitch(float deg)
        {
            pitch += deg;
            //transform *= btTransform(btQuaternion(btVector3(0, 1, 0), deg));
        }

        void Camera::tumbleRoll(float deg)
        {
            roll += deg;
            //transform *= btTransform(btQuaternion(btVector3(1, 0, 0), deg));
        }
        
        void Camera::dollyIn(float meters)
        {
            dollyDist -= meters;
        }
        
        void Camera::dollyOut(float meters)
        {
            dollyDist += meters;
        }
        
        void Camera::glTransform()
        {
            glTranslatef(0,dollyDist,0);
            glRotatef(pitch, 1, 0, 0);
            glRotatef(roll, 0, 1, 0);
            glRotatef(yaw, 0, 0, 1);
            /*
            btTransform transform(btQuaternion(roll, pitch, yaw));
            btScalar glMatrix[16];
            transform.inverse().getOpenGLMatrix(glMatrix);
#ifdef BT_USE_DOUBLE_PRECISION
            glMultMatrixd(glMatrix);
#else
            glMultMatrixf(glMatrix);
#endif
             */
        }
        
    }
}