function bRn = bRn(theta)
%BRN Rotates v_n from inertial frame to body frame
bRn = [cos(theta),sin(theta);-sin(theta),cos(theta)];
end

