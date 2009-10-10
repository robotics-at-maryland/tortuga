module ram {
	module tortuga {
		interface SensorBoard {
			idempotent void SetText(string s);
		};
	};
};

