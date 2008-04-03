function Fthrust = ObserverController_LQGIntegrater(y,xd,dt)

global A_c;
global B_c;
global C_c;
global xHat4;

%compensator dynamics at time i-1
%xHat4_dot = A_c*xHat4 + B_c*y;
xHat4_dot = A_c*xHat4 + B_c*(y-xd);

%integrate compensator dynamics to time i
xHat4 = xHat4 + xHat4_dot*dt;

%Fthrust = C_c*(xHat4-[0 xd 0 0]');
Fthrust = C_c*xHat4;
