function waveEquationIntegrate
% Solution of the wave equation by direct integration.
% All the work is done inside waveEquationUpdate, which
% will work for a wave equation in any number of dimensions.
% The sample setup below is in 2D.
%
% NOTE ABOUT UNITS.
%  Time and distance scale are folded into the parameter
% csq, which stands for "c squared", or speed of sound squared.
% If the time step is dt, and the distance scale is dx, and the
% speed of sound is c, then csq is given by:
%
%   csq = c^2 * dt^2 / dx^2
%

  % Number of iterations
  nIters = 300;
  
  % Set up initial pressure field
  P = zeros(100);
  Pold = P;
  % Place a patch of white noise in the center
  P(40:50,48:52) = rand(1);

  % Set up speed of sound squared table
  % Slow air-like layer above
  % Fast water-like layer below
  csq = ones(size(P))*0.4;
  csq(1:20,:) = 0.02;
  % Make edges of screen totally reflective
  csq(1,:) = 0;
  csq(:,1) = 0;
  csq(end,:) = 0;
  csq(:,end) = 0;
  
  % Run simulation
  for i = 1:nIters
    % Update pressure field
    [P, Pold] = waveEquationUpdate(P, Pold, csq);
    % Render
    imagesc(P);
    caxis([-1 1]);
    drawnow;
  end
end

function [Pnew, P] = waveEquationUpdate(P, Pold, csq)
% Update the solution to the wave equation by forward integration.
% Input paramters:
%   P     The pressure at the current time
%   Pold  The pressure at the previous time
%   csq   A scalar or table describing the speed of sound squared in the
%         medium at all points.
% Output parameters:
%   Pnew  The pressure at the next time step
%   P     The pressure at the current time step
  nDimensions = length(size(P));
  Pnew = 2 * P .* (1 - csq * nDimensions) - Pold;
  for dimension=1:nDimensions
    shiftSpec = zeros(1, nDimensions);
    shiftSpec(dimension) = 1;
    Pnew = Pnew + csq .* circshift(P, shiftSpec);
    Pnew = Pnew + csq .* circshift(P, -shiftSpec);
  end
end
