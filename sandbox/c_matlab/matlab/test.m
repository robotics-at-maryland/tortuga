% Test of setup to get matlab to work better with controllers

% You will have to edit these
addpath('C:\Documents and Settings\VPS\Desktop\AUV\Sandbox\c_matlab\scripts\C_Matlab\bin\Release')
loadlibrary('C_Matlab',@interface)

testStruct = struct('depth', 1, 'q0', 1, 'q1', 1, 'q2', 1, 'q3', 1);
testStructPtr = libpointer('TestStruct', testStruct);

% Call library and grab results
calllib('C_Matlab', 'testStruct', testStructPtr);

teststruct = get(testStructPtr, 'Value')

% Test Desired State
DesiredState = struct('speed',1,'depth',1,'quaternion',[1 2 3 4]', ...
                      'angularRate',[1 2 3]');
                      
desiredState = CppDesiredState(DesiredState,1);
desiredStatePtr = libpointer('DesiredState', desiredState);
calllib('C_Matlab', 'testDesired', desiredStatePtr);

desired = get(desiredStatePtr, 'Value')


% Test Measured State
MeasuredState = struct('depth',1,'linearAcceleration',[1 2 3]', ...
    'quaternion',[1 2 3 4]','angularRate',[1 2 3]');
                      
measuredState = CppMeasuredState(MeasuredState,1);
measuredStatePtr = libpointer('MeasuredState', measuredState);
calllib('C_Matlab', 'testMeasured', measuredStatePtr);

measured = get(measuredStatePtr, 'Value')