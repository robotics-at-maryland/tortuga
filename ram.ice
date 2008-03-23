module ram
{
	
	module math
	{
		class Vector {
			idempotent double Dot(Vector v);
			idempotent double Norm();
			idempotent double NormSquared();
			
			idempotent Vector Normalize();
			idempotent Vector Add(Vector v);
			idempotent Vector Subtract(Vector v);
			idempotent Vector Multiply(double s);
			idempotent Vector Negate();
			idempotent Vector CrossProduct(Vector v);
			
			double x;
			double y;
			double z;
		};
		
		class Quaternion
		{
			idempotent double Dot(Quaternion q);
			idempotent double Norm();
			idempotent double NormSquared();
			
			idempotent Quaternion Normalize();
			idempotent Quaternion Add(Quaternion q);
			idempotent Quaternion Subtract(Quaternion q);
			idempotent Quaternion MultiplyQuaternion(Quaternion q);
			idempotent Quaternion MultiplyScalar(double s);
			idempotent Quaternion Inverse();
			idempotent Quaternion Negate();
			
			idempotent Vector MultiplyVector(Vector v);
			
			double w;
			double x;
			double y;
			double z;
		};
	};
	
	module control
	{
		interface Controller
		{
			/**
			 * Set the current speed, clamped between -5 and 5
			 */
			void setSpeed(double speed);
			
			/**
			 * Sets the desired depth of the sub in meters
			 */
			void setDepth(double depth);

			/**
			 * Gets the current speed, a value between -5 and 5
			 */
			idempotent double getSpeed();

			/**
			 * Current desired depth of the sub in meters
			 */
			idempotent double getDepth();
			
			/**
			 * Grab current estimated depth
			 */
			double getEstimatedDepth();
			
			/**
			 * Grab current estimated depth velocity (depthDot)
			 */
			double getEstimatedDepthDot();

			/**
			 * Yaws the desired vehicle state by the desired number of degrees
			 */
			void yawVehicle(double degrees);

			/**
			 * Pitches the desired vehicle state by the desired number of degrees
			 */
			void pitchVehicle(double degrees);

			/**
			 * Rolls the desired vehicle state by the desired number of degrees
			 */
			void rollVehicle(double degrees);

			/**
			 * Gets the current desired orientation
			 */
			idempotent math::Quaternion getDesiredOrientation();
			
			/**
			 * Returns true if the vehicle is at the desired depth
			 */
			idempotent bool atDepth();
			
			/**
			 * Returns true if the vehicle is at the desired orientation
			 */
			idempotent bool atOrientation();
			
		};
	};
	
};
