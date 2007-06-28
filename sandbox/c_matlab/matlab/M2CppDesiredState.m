function cppDesiredState = M2CppDesiredState(DesiredState)

q = DesiredState.quaternion;
w = DesiredState.angularRate;

% Convert static array to individual numbers (HACK!)
cppDesiredState = struct('speed',DesiredState.speed, ...
                         'depth',DesiredState.depth, ...
                         'q0', q(1), 'q1', q(2), 'q2', q(3), 'q3', q(4),  ...% quaternion[4]
                         'w0', w(1), 'w1', w(2), 'w2', w(3)); % angularRate[3]

