function Fthrust = ObserverController_LQGIntegrater(y,xd,dt)

global A_c;
global B_c;
global C_c;
global W;

W_dot = A_c*W + B_c*y;
W = W + W_dot*dt;
Fthrust = C_c*(W-[xd 0 0 0]');