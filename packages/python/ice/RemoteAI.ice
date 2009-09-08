/**
 * Ice definition file
 *
 * WARNING: When you reslice this file using slice2py,
 * there are some import issues because of how the modules are set up.
 * To fix them, go into RemoteAI_ice.py and change this line:
 *     _M_RemoteAi = Ice.openModule('RemoteAi')
 * to
 *     _M_RemoteAi = Ice.openModule('ice.RemoteAi')
 * Then enter RemoteAi/__init__.py and remove the line that says:
 *     import RemoteAI_ice
 * In the same file, if this line doesn't already exist, add it.
 *     import ice.RemoteAI_ice
 */

module RemoteAi {
	struct StateEvent {
		long timeStamp;
		string type;
		string name;
		string eventType;
	};

	sequence<StateEvent> StateList;

	interface AiInformation {
		void RecordEvent(StateEvent event);
		idempotent StateList GetInfo(long seconds);
	};
};

