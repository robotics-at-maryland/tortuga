module ram {
	module tortuga {
		interface SensorBoard {
			idempotent void SetText(string line1, string line2);
		};
	};
};
