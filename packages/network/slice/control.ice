
module proxy {
    module math {
        struct Vector2 {
            double x;
            double y;
        };

        struct Quaternion {
            double x;
            double y;
            double z;
            double w;
        };
    };

    module control {
        interface IController {
            void setVelocity(math::Vector2 velocity);
            math::Vector2 getVelocity();

            void setSpeed(double speed);
            void setSidewaysSpeed(double speed);

            double getSpeed();
            double getSidewaysSpeed();

            void holdCurrentPosition();

            void setDesiredVelocity(math::Vector2 velocity, int frame);
            void setDesiredPosition(math::Vector2 position, int frame);
            void setDesiredPositionAndVelocity(math::Vector2 position,
                                               math::Vector2 velocity);
            math::Vector2 getDesiredVelocity(int frame);
            math::Vector2 getDesiredPosition(int frame);

            bool atPosition();
            bool atVelocity();

            void yawVehicle(double degrees);
            void pitchVehicle(double degrees);
            void rollVehicle(double degrees);

            math::Quaternion getDesiredOrientation();
            void setDesiredOrientation(math::Quaternion quat);
            bool atOrientation();

            void setDepth(double depth);
            double getDepth();

            double getEstimatedDepth();
            double getEstimatedDepthDot();
            bool atDepth();
            void holdCurrentDepth();

            void holdCurrentHeading();
        };
    };
};
