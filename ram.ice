module ram
{
	
    module core
    {
        class TimeVal
        {
            // Methods not yet implemented
            /*
            idempotent long seconds();
            idempotent long microseconds();
            idempotent long getDouble();
            idempotent bool equal(TimeVal other);
            */
        };
    };
    
	module math
	{
		class Vector3 {
            // Methods not yet implemented
            /*
			idempotent double Dot(Vector3 v);
			idempotent double Norm();
			idempotent double NormSquared();
			
			idempotent Vector3 Normalize();
			idempotent Vector3 Add(Vector3 v);
			idempotent Vector3 Subtract(Vector3 v);
			idempotent Vector3 Multiply(double s);
			idempotent Vector3 Negate();
			idempotent Vector3 CrossProduct(Vector3 v);
			*/
			double x;
			double y;
			double z;
		};
		
		class Quaternion
		{
            // Methods not yet implemented
            /*
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
			*/
            
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
        interface IDevice
        {
            idempotent string getName();
        };
        
        interface ICurrentProvider extends IDevice
        {
            idempotent double getCurrent();
        };
        
        interface IDepthSensor extends IDevice
        {
            idempotent double getDepth();
        };
        
        interface IIMU extends IDevice
        {
            idempotent math::Vector3 getLinearAcceleration();
            idempotent math::Vector3 getAngularRate();
            idempotent math::Quaternion getOrientation();
        };
        
        interface IMarkerDropper extends IDevice
        {
            void dropMarker();
        };
        
        interface IPowerSource extends IDevice
        {
            idempotent bool isEnabled();
            idempotent bool inUse();
            void setEnabled(bool state);
        };
        
        sequence<double> DoubleSeq;
        sequence<string> StringSeq;
        interface IPSU extends IDevice
        {
            idempotent double getBatteryVoltage();
            idempotent double getBatteryCurrent();
            idempotent double getTotalWattage();
            idempotent DoubleSeq getVoltages();
            idempotent DoubleSeq getCurrents();
            idempotent DoubleSeq getWattages();
            idempotent StringSeq getSupplyNames();
        };
        
        interface ISonar extends IDevice
        {
            idempotent math::Vector3 getDirection();
            idempotent double getRange();
            idempotent core::TimeVal getPingTime();
        };
        
        interface ITempSensor extends IDevice
        {
            idempotent int getTemp();
        };
        
        interface IThruster extends IDevice
        {
            void setForce(double newtons);
            idempotent double getForce();
            idempotent double getMaxForce();
            idempotent double getMinForce();
            idempotent double getOffset();
            idempotent bool isEnabled();
            void setEnabled(bool tf);
        };
        
        dictionary<string, IDevice*> DeviceDictionary;
		interface IVehicle
		{
            idempotent DeviceDictionary getDevices();
            idempotent double getDepth();
            idempotent math::Vector3 getLinearAcceleration();
            idempotent math::Vector3 getAngularRate();
            idempotent math::Quaternion getOrientation();
            void applyForcesAndTorques();
            void safeThrusters();
            void unsafeThrusters();
            void dropMarker();
		};
        
        interface IVoltageProvider extends IDevice
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
