%Joe's attempt to learn how to use Structs in Matlab

%define structs that will be used in real program
DesiredState = struct('speed',1,'depth',1,'quaternion',[1 2 3 4]','angularRate',[1 2 3]');
MeasuredState = struct('depth',1,'linearAcceleration',[1 2 3]','quaternion',[1 2 3 4]','angularRate',[1 2 3]');
ControllerState = struct('estimatedInertias',[1 2 3; 4 5 6; 7 8 9],'angularPGain',1,'angularDGain',1)

%set desired state
DesiredState.speed = 3;
DesiredState.depth = 8;
DesiredState.quaternion = [1 0 0 1]';
DesiredState.angularSpeed = [3 3 -1]';

%set measured state
MeasuredState.depth = 12;
MeasuredState.linearAcceleration = [-1 -1 -1]';
MeasuredState.quaternion = [-1 0 0 -1]';
MeasuredState.angularRate = [3 5 7]';

%access desired state
DesiredState.speed
DesiredState.depth
DesiredState.quaternion
DesiredState.angularSpeed

%access measured state
MeasuredState.depth
MeasuredState.linearAcceleration
MeasuredState.quaternion
MeasuredState.angularRate