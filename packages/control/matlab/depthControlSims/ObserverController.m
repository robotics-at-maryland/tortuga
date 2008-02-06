function Fthrust = ObserverController(y,xd,dt)
% Returns force output from the observer controller
A = [0 1;0 0];
B = [0; 1];
C = [1 0];


global x_hat;
global K;
global L;

% y_hat: estimation of y from previous state estimate
y_hat = x_hat(1);

% finding x_dot_prev from x_prev and y
x_dot_prev = A*x_hat + B*(-(K)*x_hat) + (L)*(y-y_hat);

% x_hat: calculate estimation of new x from x_prev and x_dot_prev
x_hat = x_hat + dt*x_dot_prev;

% Fthrust (U) : new control signal
Fthrust = -(K)*(x_hat-[xd; 0]);
end