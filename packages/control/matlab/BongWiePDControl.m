function [rotationalTorques aHatNew]=BongWiePDControl(MeasuredState,DesiredState,ControllerState,dt)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% [rotationalTorques
% aHatNew]=BongWiePDControl(MeasuredState,DesiredState,ControllerState,dt)
%
% function BongWiePDControl.m is an encapsulation of the
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
estimatedInertia=[aHat(1) aHat(2) aHat(3);
                  aHat(2) aHat(4) aHat(5);
                  aHat(3) aHat(2) aHat(6)];
%proportional controller gain matrix
PGain=1*estimatedInertia;
%derivative controller gain matrix
DGain=1*estimatedInertia;

%QUATERNION is NOT a quaternion, rather it is a matrix composed of
%parts of a quaternion.  it is later used to find the difference
%between the quaternion q_goal and the vehicle's orientation q
QUATERNION = [qDesired(4) qDesired(3) -qDesired(2) -qDesired(1);
              -qDesired(3) qDesired(4) qDesired(1) -qDesired(2);
              qDesired(2) -qDesired(1) qDesired(4) -qDesired(3);
              qDesired(1) qDesired(2) qDesired(3) qDesired(4)];
%a skew symmetric matrix composed of the vehicle's angular velocity.
%this is used later to facilitate the cross product between the w
%vector and J*w
wTilde = [0 -w(3) w(2); w(3) 0 -w(1); -w(2) w(1) 0];
%an error quaternion, it is SORT of like the difference between the
%desired and actual quaternion, but quaternions can't simply be added
%and subtracted from one another
qError=QUATERNION*q; 
%angular velocity error, simply the difference between the desired and
%actual angular position
wError=w-[0 0 0]'; 

%controller
%gyro
rotationalTorques=-DGain*wError-PGain*qError(1:3)+wTilde*estimatedInertia*w; %compute control torques
%no gyro
%rotationalTorques=-DGain*wError-PGain*qError(1:3); %compute control torques

%to conform with function definition
aHatNew = aHat;

