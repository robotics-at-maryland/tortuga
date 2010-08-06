function dx = positionHoldSimDynamics(t,x)
%all dynamics simulated in inertial frame

%% input
%parse state vector
%position (m) and velocity (m/s) in inertial frame
%orientation (rad) is angle from inertial frame to body frame
%state = [r1 r2 r1Dot r2Dot theta thetaDot]

r=[x(1) x(2)]';
r_dot=[x(3) x(4)]';
theta=x(5);
theta_dot=x(6);

%pull in system constants
global m;%mass (kg)
global h;%inertial about yaw axis (N*m*s^2)
global drag_1;%translational drag along 1 axis
global drag_2;%translational drag along 2 axis
global drag_rot;%rotational drag about yaw axis

%pull in forces and torques from controller
global Fn_controller;
global Fn_disturbances;
global Tn_controller;
global Tn_disturbances;


%% diff eqs
r1_dotdot=1/m*(Fn_controller(1) - drag_1*r_dot(1) + Fn_disturbances(1));
r2_dotdot=1/m*(Fn_controller(2) - drag_2*r_dot(2) + Fn_disturbances(2));

theta_dotdot=1/h*(Tn_controller - drag_rot*theta_dot + Tn_disturbances);

dx=[r_dot(1) r_dot(2) r1_dotdot r2_dotdot theta_dot theta_dotdot]';
