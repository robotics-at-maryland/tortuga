function [rotationalTorques aHatNew]=rotationalController(MeasuredState,DesiredState,ControllerState,dt)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% [rotationalTorques
% aHatNew]=rotationalController(MeasuredState,DesiredState,ControllerState,dt)
%
% function rotationalController.m is an encapsulation of the
% rotational controller
%
% assumes the quaternion is written as 
%
% q = [e(0)*sin(phi/2); e(1)*sin(phi/2); e(2)*sin(phi/2); cos(phi/2)]
%
% for simplicity assumes desired angular velocity is zero
%
% 
% NOTE: this function changes the inertia estimate (as it should!!!)
%
% returns 
%           - rotationalTorques, the torques used to rotate the vehicle as
%             written in the vehicle's coord frame.  torques are in Newton*meters
%           - aHatNew, the new estimate of the adaptation parameters
%
%
% written by Joseph Gland
% June 22 2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%unpack data from states for legibility
%measured quaternion
q=MeasuredState.quaternion;
%desired quaternion
qDesired=DesiredState.quaternion;
%measured angular velocity
w=MeasuredState.angularRate;
%inertia estimates
aHat=ControllerState.inertiaEstimate;
%controller gain matrix
Kd=ControllerState.angularGain;
%adaptation gain matrix
gamma=ControllerState.adaptationGain;
%

%quaternion pseudo error metric
qTilde=[qDesired(4)*eye(3)-S(qDesired(1:3)) -qDesired(1:3); qDesired(1:3)' qDesired(4)]*q;
%quaternion pseudo error metric derivative
qTildeDot=(1/2)*[qTilde(4)*eye(3)+S(qTilde(1:3)); -qTilde(1:3)']*w;
%angular rate pseudo error metric
wr=-qTilde(1:3);
%composite error metric
s=w-wr;
%adaptation metric
Y=[-qTildeDot(1) -qTildeDot(2) -qTildeDot(3) 0 0 0;
   0 -qTildeDot(1) 0 -qTildeDot(2) -qTildeDot(3) 0;
   0 0 -qTildeDot(1) 0 -qTildeDot(2) -qTildeDot(3)];

%adaptive control law
%rotationalTorques=-Kd*s+Y*aHat;
%boring PD control law, adaptive law freaking out
rotationalTorques=-Kd*s;    

%find derivatives of adaptive parameters
%gamma
%Y
%s
aHatDot = -gamma*Y'*s;
%integrate
%code that I WANT to put in, but matlab has permission issues with structs
%ControllerState.inertiaEstimate = aHat + aHat*dt;
%ControllerState.inertiaEstimate
%stupid code I have to use (blegh!)
aHatNew = aHat + aHat*dt;