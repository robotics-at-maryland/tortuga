module ram
{
	module marcopolo
	{
		sequence<short> ShortList;
		
		struct Snapshot
		{
			ShortList samples;
            short triggerLevel;
		};
		
		class Polo
		{
			void reportData(Snapshot snap);
            void triggerChanged(short newLevel);
		};
		
		class Marco
		{
			idempotent void setChannelEnabled(int channel, bool enabled);
			idempotent bool getChannelEnabled(int channel);
            
            idempotent void setTriggerEdge(bool rising);
            idempotent void setTriggerChannel(int channel);
            
            void incrementTriggerLevel(short amount);
            void decrementTriggerLevel(short amount);
            
			idempotent void registerPolo(Polo* p);
		};
	};
};
