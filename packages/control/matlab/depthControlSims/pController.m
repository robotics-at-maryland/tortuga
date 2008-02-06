function Fthrust = pController(x_measured,x_desired)
% Returns force output from the proportional controller

global kp;

Fthrust=-kp*(x_measured-x_desired);