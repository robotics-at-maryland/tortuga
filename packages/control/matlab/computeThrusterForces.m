function thrusterForces=computeThrusterForces(translationalForces,rotationalTorques)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% thrusterForces=computeThrusterForces(translationalForces,rotationalTorques)
%
% output: thrusterForces = [FPort; FStarboard; FFore; FAft];
%
% WARNING!!!! trans and rot sloppily thrown together!!! fix later!!!
%
% assumes the quaternion is written as 
%
% q = [e(0)*sin(phi/2); e(1)*sin(phi/2); e(2)*sin(phi/2); cos(phi/2)]
%
%
% returns thrusterForces, the combination of the rotational and
% translational forces as requested by the respective controllers and
% mapped to the thrusters on the sub, where:
%
% thrusterForces = [FPort; FStarboard; FFore; FAft]
%
% assumes there are 4 thrusters:
% port and starboard thrusters both pointing to the front of the vehicle
% and fore and aft thrusters both pointing to the bottom of the vehicle
%
% assumes translationalForces in newtons
% assumes rotationalTorques in newton*meters
%
% written by Joseph Gland
% June 24 2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%measure these distances by hand
%these distances are from the CG to the center of the thrusters
%these should be in meters
rPort = 0.1905;
rStarboard = 0.1905;
rFore = 0.3366;
rAft = 0.3366;

%todo: make this code not shitty
FPort = translationalForces(1)/2-0.5*rotationalTorques(3)/rPort;
FStarboard = translationalForces(1)/2+0.5*rotationalTorques(3)/rStarboard;
FFore = translationalForces(3)/2+0.5*rotationalTorques(2)/rFore;
FAft = translationalForces(3)/2-0.5*rotationalTorques(2)/rAft;

thrusterForces = [FPort; FStarboard; FFore; FAft];