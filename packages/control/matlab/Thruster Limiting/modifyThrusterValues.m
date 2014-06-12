function val = modifyThrusterValues(thruster_vals, thruster_max)

%[front back top bottom right left] = getForceValues(F1, F2, F3, T1, T2, T3)

% forward left up roll pitch yaw

rSide = .5;
rFront = 1.5;
rTop = .5;

val = zeros(size(thruster_vals));
t_ratio = 1;

[M N] = size(thruster_vals);

for x = 1:N
    thrusters = thruster_vals(:,x);
    
    % Find max thruster value
    [value index] = max(thrusters);
    
    if (value > thruster_max)
        
        t_ratio = thruster_max / thrusters(index);

        % reduce each thruster value by t_ratio
        thrusters = thrusters*t_ratio;

        thrusters

        % Find new Torques (roll,pitch,yaw)
        forces = getForceValues(thrusters(1), thrusters(2), thrusters(3), thrusters(4), thrusters(5), thrusters(6));
        %T1 = 1.5 * (1*rTop*thrusters(3) + rTop*thrusters(4))
        %T2 = 1.5 * (-1*rFront*thrusters(1) + rFront*thrusters(2))
        %T3 = 1.5 * (-1*rSide*thrusters(6) + rSide*thrusters(5))
        T1 = forces(4);
        T2 = forces(5);
        T3 = forces(6);

        T1= T1*1.5;
        T2= T2*1.5;
        T3= T3*1.5;
        % Find new thruster values for Thrusters 4, 2, and 5
        thrusters(4) = T1/rTop - thrusters(3);
        thrusters(2) = T2/rFront + thrusters(1);
        thrusters(5) = T3/rSide + thrusters(6);

        % Find new Forces (forward,left,down)
        %F1 = 2 * (thrusters(6) + (1/rSide)*T3);
        %F2 = 2 * (thrusters(3) + (1/rTop)*T1);
        %F3 = 2 * (thrusters(1)  + (1/rFront)*T2)

        %     thrusters(2) = .5*F3 + (1/rFront)*T2;
        %     thrusters(5) = .5*F1 + (1/rSide)*T3;
        %     thrusters(4) = .5*F2 + (1/rTop)*T1;


        thrusters;
    end
    val(:,x) = thrusters;


    if(max(val(:,x)) > thruster_max)
        ratio = thruster_max / max(val(:,x));
        val(:,x) = val(:,x) * ratio;
    end
end

val;