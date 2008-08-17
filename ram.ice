module ram
{
	
    module core
    {
        class TimeVal
        {
            idempotent long seconds();
            idempotent long microseconds();
            idempotent long getDouble();
            idempotent bool equal(TimeVal other);
        };
    };
    
	module math
	{
		class Vector3 {
			idempotent double Dot(Vector3 v);
			idempotent double Norm();
			idempotent double NormSquared();
			
			idempotent Vector3 Normalize();
			idempotent Vector3 Add(Vector3 v);
			idempotent Vector3 Subtract(Vector3 v);
			idempotent Vector3 Multiply(double s);
			idempotent Vector3 Negate();
			idempotent Vector3 CrossProduct(Vector3 v);
			
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
			
			idempotent Vector3 MultiplyVector(Vector3 v);
			
			double w;
			double x;
			double y;
			double z;
		};
	};
	
	module control
	{
		interface IController
		{
			void setSpeed(double speed);
			void setDepth(double depth);
			idempotent double getSpeed();
			idempotent double getDepth();
			double getEstimatedDepth();
			double getEstimatedDepthDot();
			void yawVehicle(double degrees);
			void pitchVehicle(double degrees);
			void rollVehicle(double degrees);
			idempotent math::Quaternion getDesiredOrientation();
			idempotent bool atDepth();
			idempotent bool atOrientation();
		};
	};
	
	module vehicle
	{
        interface ICurrentProvider
        {
            idempotent double getCurrent();
        };
        
        interface IDepthSensor
        {
            idempotent double getDepth();
        };
        
        interface IIMU
        {
            idempotent math::Vector3 getLinearAcceleration();
            idempotent math::Vector3 getAngularRate();
            idempotent math::Quaternion getOrientation();
        };
        
        interface IMarkerDropper
        {
            void dropMarker();
        };
        
        interface IPowerSource
        {
            idempotent bool isEnabled();
            idempotent bool inUse();
            void setEnabled(bool state);
        };
        
        sequence<double> DoubleSeq;
        sequence<string> StringSeq;
        interface IPSU
        {
            idempotent double getBatteryVoltage();
            idempotent double getBatteryCurrent();
            idempotent double getTotalWattage();
            idempotent DoubleSeq getVoltages();
            idempotent DoubleSeq getCurrents();
            idempotent DoubleSeq getWattages();
            idempotent StringSeq getSupplyNames();
        };
        
        interface ISonar
        {
            idempotent math::Vector3 getDirection();
            idempotent double getRange();
            idempotent core::TimeVal getPingTime();
        };
        
        interface ITempSensor
        {
            idempotent int getTemp();
        };
        
        interface IThruster
        {
            void setForce(double newtons);
            idempotent double getForce();
            idempotent double getMaxForce();
            idempotent double getMinForce();
            idempotent double getOffset();
            idempotent bool isEnabled();
            void setEnabled(bool tf);
        };
        
		interface IVehicle
		{
            idempotent double getDepth();
            idempotent math::Vector3 getLinearAcceleration();
            idempotent math::Vector3 getAngularRate();
            idempotent math::Quaternion getOrientation();
            void applyForcesAndTorques();
            void safeThrusters();
            void unsafeThrusters();
            void dropMarker();
		};
        
        interface IVoltageProvider
        {
            idempotent double getVoltage();
        };
        
        exception CannotCreateException
        {
            string reason;
        };
		
		interface IVehicleFactory
		{
			IVehicle* getVehicleByName(string vehicleName)
                throws CannotCreateException;
		};
	};
	
};
