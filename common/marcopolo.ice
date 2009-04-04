module ram
{
	module marcopolo
	{
		sequence<short> ShortList;
		
		struct Snapshot
		{
			ShortList samples;
		};
		
		class Polo
		{
			void reportData(Snapshot snap);
		};
		
		class Marco
		{
			idempotent void setChannelEnabled(int channel, bool enabled);
			idempotent bool getChannelEnabled(int channel);
			idempotent void setTrigger(int channel, short level);
			idempotent void registerPolo(Polo* p);
		};
	};
};
