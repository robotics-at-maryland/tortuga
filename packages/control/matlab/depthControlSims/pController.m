function Fthrust = pController(kp,x_measured,x_desired)
% Returns force output from the proportional controller

Fthrust=-kp*(x_measured-x_desired);