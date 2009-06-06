module ram {
	module events {
		
		struct Vector3
		{
			double x;
			double y;
			double z;
		};
		
		struct SonarEvent
		{
			Vector3 direction;
			double range;
			int pingTimeSec;
			int pingTimeUSec;
		};
		
		interface SonarEventListener
		{
			void pingReceived(SonarEvent e);
		};
		
	};
};
