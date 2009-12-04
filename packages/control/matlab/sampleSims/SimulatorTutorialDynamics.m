function dx = SimulatorTutorialDynamics(t,x)

%unpack
x1=x(1);
x2=x(2);

%Import global variables
global N1;
global N2;
global k;
global m;
global c;
global gain1;
global gain2;

%measurement
%  These measurements for position(x1) and velocity(x2) account for
%  error (using rand*N).  
x1meas=x1+rand*N1;
x2meas=x2+rand*N2;

%control
u=-gain1*x1meas-gain2*x2meas;
%u = Net force exerted by the rockets

%kinematics
xdot(1) = x2;
%dynamics
xdot(2) = -(k/m)*x1 -(c/m)* x2 + (1/m)* u;

%pack
dx=[xdot(1); xdot(2)];
