function val = modifyThrusterValues3(thruster_vals, thruster_max)

%[front back top bottom right left] = getForceValues(F1, F2, F3, T1, T2, T3)

% forward left up roll pitch yaw

rSide = .5;
rFront = 1.5;
rTop = .5;

val = thruster_vals;


for x = 1:length(thruster_vals)
    thrusters = thruster_vals(:,x);

    if(max(thrusters) > thruster_max)
        ratio = thruster_max / max(thrusters);
        val(:,x) = thrusters * ratio;
    end
    
    
end

val;