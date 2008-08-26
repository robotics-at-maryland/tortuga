#ifndef _RAM_SIM_CAMERA_H
#define _RAM_SIM_CAMERA_H

#include <btBulletDynamicsCommon.h>

namespace ram {
    namespace sim {
        class Camera {
        private:
            float yaw, pitch, roll, dollyDist, trackLeftDist, trackUpDist;
        public:
            Camera();
            
            /// Tumble the camera about the +z axis
            void tumbleYaw(float degrees);
            
            /// Tumble the camera about the +y axis.
            void tumblePitch(float degrees);
            
            /// Tumble the camera about the +x axis.
            void tumbleRoll(float degrees);
            
            /// Dolly in (move straight into the screen)
            void dollyIn(float meters);
            
            /// Dolly out (move straight out of the screen)
            void dollyOut(float meters);
            
            /// Track the camera left (scene appears to move right on the screen)
            void trackLeft(float meters);
            
            /// Track the camera right (scene appears to move left on the screen)
            void trackRight(float meters);
            
            /// Track the camera up (scene appears to move down on the screen)
            void trackUp(float meters);
            
            /// Track the camera down (scene appears to move up on the screen)
            void trackDown(float meters);
            
            
            /// Transform the current OpenGL matrix to look through this camera
            void glTransform();
        };
    }
}

#endif
