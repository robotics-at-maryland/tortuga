function LimitedThrusterForces = thrusterSaturationModel(ThrusterForces)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% LimitedThrusterForces = thrusterSaturationModel(ThrusterForces)
%
% simulates real thrusters with finite thrust force
% magic numbers are from thruster calibration data
%
% input: ThrusterForces - a 4x1 vector of forces desired from the thrusters
%
% output: LimitedThrusterForces - ThrusterForces passed through a
%                                       saturation model
%
% written by Joseph Gland
% June 28, 2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%maximum thrust in Newtons
maximumThrust = 10;

for index=1:1:4 
    if ThrusterForces(index) > maximumThrust
        LimitedThrusterForces(index) = maximumThrust;
    elseif ThrusterForces(index) < -maximumThrust
        LimitedThrusterForces(index) = -maximumThrust;
    else
        LimitedThrusterForces(index) = ThrusterForces(index);
    end
end