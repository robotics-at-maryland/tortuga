function out = ObserverController(y,mu_controller,mu_observer,dt)
% Returns force output from the observer controller
A = [0 1;0 0];
B = [0; 1];
C = [1 0];
K = place(A,B,mu_controller);%do this in a separate file
L = (place(A',C',mu_observer))';%do this in a separate file

global x_hat;

% y_hat: estimation of y from previous state estimate
y_hat = x_hat(1);

% finding x_dot_prev from x_prev and y
x_dot_prev = A*x_hat + B*(-K*x_hat) + L*(y-y_hat);

% x_hat: calculate estimation of new x from x_prev and x_dot_prev
x_hat = x_hat + dt*x_dot_prev;

% Fthrust (U) : new control signal
Fthrust = -K*x_hat;

out = Fthrust;

end