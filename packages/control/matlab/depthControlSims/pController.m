function Fthrust = pController(x_measured,x_desired,dt)
% Returns force output from the proportional controller

global kp;

Fthrust=-kp*(x_measured-x_desired);