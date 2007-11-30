function Fthrust = pdController(kp,kd,x_measured,x_desired,x2_measured)
% Returns force output from the proportional and derivative controller

Fthrust=-kp*(x_measured-x_desired)-kd*(x2_measured);