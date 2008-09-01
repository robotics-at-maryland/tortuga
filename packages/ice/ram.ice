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
			double x;
			double y;
			double z;
			double w;
		};
	};
	
	module control
	{
		interface Controller
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
		
        interface Device
        {
            idempotent string getName();
        };
        
        interface CurrentProvider extends Device
        {
            idempotent double getCurrent();
        };
        
        interface VoltageProvider extends Device
        {
            idempotent double getVoltage();
        };
        
        interface PowerSource extends CurrentProvider, VoltageProvider
        {
            idempotent bool isEnabled();
            idempotent bool inUse();
            void setEnabled(bool state);
        };
        
        interface DepthSensor extends Device
        {
            idempotent double getDepth();
        };
        
        interface IMU extends Device
        {
            idempotent transport::Vector3 getLinearAcceleration();
            idempotent transport::Vector3 getAngularRate();
            idempotent transport::Quaternion getOrientation();
        };
        
        interface MarkerDropper extends Device
        {
            void dropMarker();
        };
        
        interface Sonar extends Device
        {
            idempotent transport::Vector3 getDirection();
            idempotent double getRange();
            idempotent core::TimeVal getPingTime();
        };
        
        interface TempSensor extends Device
        {
            idempotent int getTemp();
        };
        
        interface Thruster extends Device
        {
            void setForce(double newtons);
            idempotent double getForce();
            idempotent double getMaxForce();
            idempotent double getMinForce();
            idempotent double getOffset();
            idempotent bool isEnabled();
            void setEnabled(bool tf);
        };
        
        sequence<Device*> DeviceList;
		interface IVehicle
		{
            idempotent vehicle::DeviceList getDevices();
            idempotent vehicle::Device* getDeviceByName(string name)
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
		interface VehicleFactory
		{
			IVehicle* getVehicleByName(string vehicleName)
                throws CannotCreateException;
		};
	};
	
};
