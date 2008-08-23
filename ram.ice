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
    
	module transport
	{
		class Vector3 {
			double x;
			double y;
			double z;
		};
		
		class Quaternion
		{
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
			idempotent transport::Quaternion getDesiredOrientation();
			idempotent bool atDepth();
			idempotent bool atOrientation();
		};
	};
	
	module vehicle
	{
        exception DeviceNotFoundException
        {
            string reason;
        };
		
        interface IDevice
        {
            idempotent string getName();
        };
        
        interface ICurrentProvider extends IDevice
        {
            idempotent double getCurrent();
        };
        
        interface IVoltageProvider extends IDevice
        {
            idempotent double getVoltage();
        };
        
        interface IPowerSource extends ICurrentProvider, IVoltageProvider
        {
            idempotent bool isEnabled();
            idempotent bool inUse();
            void setEnabled(bool state);
        };
        
        interface IDepthSensor extends IDevice
        {
            idempotent double getDepth();
        };
        
        interface IIMU extends IDevice
        {
            idempotent transport::Vector3 getLinearAcceleration();
            idempotent transport::Vector3 getAngularRate();
            idempotent transport::Quaternion getOrientation();
        };
        
        interface IMarkerDropper extends IDevice
        {
            void dropMarker();
        };
        
        interface ISonar extends IDevice
        {
            idempotent transport::Vector3 getDirection();
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
        
        sequence<IDevice*> DeviceList;
		interface IVehicle
		{
            idempotent DeviceList getDevices();
            idempotent IDevice* getDeviceByName(string name)
                throws DeviceNotFoundException;
            idempotent double getDepth();
            idempotent transport::Vector3 getLinearAcceleration();
            idempotent transport::Vector3 getAngularRate();
            idempotent transport::Quaternion getOrientation();
            void applyForcesAndTorques();
            void safeThrusters();
            void unsafeThrusters();
            void dropMarker();
		};
        
        exception CannotCreateException
        {
            string reason;
        };
		
        dictionary<string, IVehicle*> VehicleDictionary;
		interface IVehicleFactory
		{
			IVehicle* getVehicleByName(string vehicleName)
                throws CannotCreateException;
		};
	};
	
};
