function translationalForces = translationalController(MeasuredState,DesiredState,ControllerState,dt)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% function translationalController.m is an encapsulation of the
% translational controller
%
% assumes the quaternion is written as 
%
% q = [e(0)*sin(phi/2); e(1)*sin(phi/2); e(2)*sin(phi/2); cos(phi/2)]
%
% returns translationalForces, the forces used to translate the vehicle as
% written in the vehicle's coord frame.  forces are in newtons
%
% written by Joseph Gland
% June 22 2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



translationalForces=[0 0 0]';

%TODO: figure out how to fire thrusters for depth in crazy orientations

%depth component first calculated in inertial frame 
depthComponent=[0 0 -(ControllerState.depthPGain)*((DesiredState.depth)-(MeasuredState.depth))]';
translationalForces = rotationMatrixFromQuaternion(MeasuredState.quaternion)*depthComponent;

%speed component calculated in vehicle frame
foreAftTranslation=(ControllerState.speedPGain)*(DesiredState.speed);

translationalForces = [foreAftTranslation 0 0]' + translationalForces;