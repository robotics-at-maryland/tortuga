#include "Camera.h"
#include <SDL_opengl.h>

namespace ram {
    namespace sim {
        
        Camera::Camera() : yaw(0), pitch(0), roll(0),
        dollyDist(0), trackLeftDist(0), trackUpDist(0) {}
        
        void Camera::tumbleYaw(float deg)
        {
            yaw += deg;
        }

        void Camera::tumblePitch(float deg)
        {
            pitch += deg;
        }

        void Camera::tumbleRoll(float deg)
        {
            roll += deg;
        }
        
        void Camera::dollyIn(float meters)
        {
            dollyDist -= meters;
        }
        
        void Camera::dollyOut(float meters)
        {
            dollyDist += meters;
        }
        
        void Camera::trackLeft(float meters)
        {
            trackLeftDist += meters;
        }
        
        void Camera::trackRight(float meters)
        {
            trackLeftDist -= meters;
        }
        
        void Camera::trackUp(float meters)
        {
            trackUpDist -= meters;
        }
        
        void Camera::trackDown(float meters)
        {
            trackUpDist += meters;
        }
        
        void Camera::glTransform()
        {
            glTranslatef(trackLeftDist, dollyDist, trackUpDist);
            glRotatef(pitch, 1, 0, 0);
            glRotatef(roll, 0, 1, 0);
            glRotatef(yaw, 0, 0, 1);
        }
        
    }
}