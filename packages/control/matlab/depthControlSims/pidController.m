function Fthrust = pidController(x_measured, x_desired, dt)
% Fthrust = pidController(x_measured, x_desired, dt)
%
% an implementation of a PID controller
%
%

global kp;
global kd;
global ki;
global x_pid;

%kp = 10; kd = 4; ki = 1; x_pid = [0;0];
%kp = 14; kd = 10; ki = .5; x_pid = [-46;x_measured];
%kp = 18; kd = 10; ki = .5;
kp = 16; kd = 10; ki = .5;x_pid = [-46; x_measured];

%unpack data from x_pid storage bin
%x_pid=[0 0]';%x_pid=[sum_error previous_position]'
sum_error=x_pid(1);
previous_position=x_pid(2);

%P error
error=x_measured-x_desired;

%D error
%note that this is the same as 
%error_dot=((x_measured-x_desired)-(previous_position-x_desired))/dt
error_dot=(x_measured-previous_position)/dt;

%I error
error_int=sum_error+error*dt;


%update x_pid storage bin
x_pid=[sum_error x_measured]';

%do the actual control law
Fthrust=-kp*error-kd*error_dot-ki*error_int;
