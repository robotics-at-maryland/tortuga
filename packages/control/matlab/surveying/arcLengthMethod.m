function [ r,e ] = arcLengthMethod( m )
%ARCLENGTH Summary of this function goes here
%   takes in Measurement object
%   returns result structure
% Naming Convention
% Vectors
% {Vector Starting Point}_{Vector End Point}_{Coordinate Frame}
% Rotation Matrices
% rot{from coordinate frame}{to coordinate frame}


% Rotation matrix from Global frame to Transdec frame
rotGT = [cos(m.alpha),sin(m.alpha);-sin(m.alpha),cos(m.alpha)];
% Rotation matrix from Transdec frame to Global frame
rotTG = transpose(rotGT);

% Vector from global origin to transdec bridgeNorthEnd in the transdec frame
g_t_G = [m.bridgeNorthEndx; m.bridgeNorthEndy];
% Vector from global origin to transdec origin in the transdec frame
g_t_T = rotGT(G_T_G) + [0;d];

% Measurement input is in global frame
% Convert Measurement to transdec frame
mlocG = [m.x;m.y];
mlocT = rotGT(mlocG) + g_t_T;

% Calculate result in transdec frame

if(mlocT(2) > -(m.r-m.d)/2-m.d)
    t_x_T = m.r*sin(asin(mlocT(1)/m.r) + m.s/m.r);
    t_y_T = -m.r*cos(asin(mlocT(1)/m.r) + m.s/m.r);
else
    t_x_T = m.r*sin(asin(mlocT(1)/m.r) + m.s/m.r);
    t_y_T = -(m.r+m.d) + m.r*cos(asin(mlocT(1)/m.r) + m.s/m.r);
end

rlocT = [t_x_T;t_y_T];

% Convert the result to the global frame
rlocG = rotTG(rlocT - g_t_T);
r.x = rlocG(1);
r.y = rlocG(2);
