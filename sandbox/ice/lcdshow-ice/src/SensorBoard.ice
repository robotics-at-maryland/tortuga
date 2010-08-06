module ram {
	module tortuga {
		enum LcdCommands { lcdOn, lcdOff, lcdFlash };

		interface SensorBoard {
			idempotent void SetText(string line1, string line2);
			idempotent void ClearScreen();
			idempotent void Backlight(LcdCommands option);
			idempotent void ActivateThrusters(bool activate);
			idempotent void ActivateThrusterN(bool activate, int thruster);
			idempotent void Diagnostics(bool activate);
			idempotent void Stop();
		};
	};
};
