%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% controllerCodeSingleDataPoint.m is used to test the matlab implementation of the control
%code over a single control cycle
%
% written by Joseph Gland
% June 22 2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%initial parameters%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%define structs 
DesiredState = struct('speed',1,'depth',1,'quaternion',[1 2 3 4]','angularRate',[1 2 3]');
MeasuredState = struct('depth',1,'linearAcceleration',[1 2 3]','quaternion',[1 2 3 4]','angularRate',[1 2 3]');
ControllerState = struct('inertiaEstimate',[1 2 3 4 5 6]','adaptationGain',[1 0 0; 0 1 0; 0 0 1],'angularGain',[1 2 3; 4 5 6; 7 8 9],'depthPGain',1,'speedPGain',1);

%generate a measured quaternion
%euler parameters
e=[0; 0; 1];
e=e/norm(e);
phi = 0;
%measured quaternion
qMeasured=[e(1)*sin(phi/2);
e(2)*sin(phi/2);
e(3)*sin(phi/2);
cos(phi/2)];

%set measured state
MeasuredState.depth = 12;
MeasuredState.linearAcceleration = [-1 -1 -1]';
MeasuredState.quaternion = qMeasured;
MeasuredState.angularRate = [3 5 7]';

%generate a desired quaternion
%euler parameters
e=[0; 0; 1];%0 0 1
e=e/norm(e);
phi = pi/2;%pi/2
%original quaternion
qDesired=[e(1)*sin(phi/2);
e(2)*sin(phi/2);
e(3)*sin(phi/2);
cos(phi/2)];

%set desired state
DesiredState.speed = 3;
DesiredState.depth = 8;
DesiredState.quaternion = qDesired;
DesiredState.angularRate = [3 3 -1]';

%set controller state
ControllerState.inertiaEstimate = [1 2 3 4 5 6]';
ControllerState.adaptationGain = eye(6);
ControllerState.angularGain = [1 0 0; 0 1 0; 0 0 1];
ControllerState.depthPGain = 4;
ControllerState.speedPGain = 1;

%test translationalController function
translationalForces=translationalController(MeasuredState,DesiredState,ControllerState,dt);

%test rotationalController function
rotationalTorques=rotationalController(MeasuredState,DesiredState,ControllerState,dt);

%test computeThrusterForces function
thrusterForces=computeThrusterForces(translationalForces,rotationalTorques);

%output data
thrusterForces