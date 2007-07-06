function rotationalTorques = CppBongWiePDControl(MeasuredState, ...
                                                          DesiredState,...
                                                          ControllerState,...
                                                          dt)
                                                      
% Build CPP compatible version of structs
measuredState = CppMeasuredState(MeasuredState,1);
desiredState = CppDesiredState(DesiredState,1);
controllerState = CppControllerState(ControllerState,1);

% Create pointers to structures
measuredStatePtr = libpointer('MeasuredState', measuredState);
desiredStatePtr = libpointer('DesiredState', desiredState);
controllerStatePtr = libpointer('ControllerState', controllerState);

% Output arguments
rotationalTorques = [0,0,0]; 
rotationalTorquesPtr = libpointer('doublePtr', rotationalTorques);

% Call actuall Cpp function
calllib('Control', 'rotationalController', measuredStatePtr, ...
        desiredStatePtr, controllerStatePtr, dt, ...
        rotationalTorquesPtr);

% Grab actual values from the Cpp pointers (convert structs if needed)
rotationalTorques = get(rotationalTorquesPtr, 'Value');
measuredState = CppMeasuredState(get(measuredStatePtr, 'Value'),0);
desiredState = CppDesiredState(get(desiredStatePtr, 'Value'),0);
controllerState = CppControllerState(get(controllerStatePtr, 'Value'),0);
