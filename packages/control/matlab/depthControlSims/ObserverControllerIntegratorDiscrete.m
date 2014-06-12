function Fthrust = ObserverControllerIntegratorDiscrete(y,xd)
% TODO: document this!

global A_c;
global B_c;
global C_c;
global xHat4;



%compensator dynamics at time i-1
%xHat4_dot = A_c*xHat4 + B_c*y;
xHat4 = A_c*xHat4 + B_c*(y-xd);


Fthrust = C_c*xHat4;
