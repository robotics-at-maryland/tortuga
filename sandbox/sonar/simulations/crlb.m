function cr_lowerbound = crlb(source_pos, hydro_pos, hydro_pos_accuracy, tdoa_accuracy)
% CRLB Cramer-Rao lower bound on covariance of source coordinates
%   M = crlb(source_pos, hydro_pos, hydro_pos_accuracy, tdoa_accuracy);
%     Returns the best possible 3x3 covariance matrix between x_s, y_s,
%     and z_s, for a maximum likelihood estimator
%
%   source_pos is a row vector representing the actual coordinates of the
%   source.
%
%   Example:
%     source_pos = [1,1,1];
%
%   hydro_pos is a matrix whose rows each represent the rectangular
%   coordinates of the hydrophones.
%
%   There is always a hydrophone at (0,0,0), but it is not listed.
%   If the first row of hydro_pos is filled with zeros, it will be
%   stripped out.
%   
%   Example:
%     hydro_pos = [
%         0, sqrt(3)/3, sqrt(2/3);
%         0.5, -sqrt(3)/6, sqrt(2/3);
%         -0.5, -sqrt(3)/6, sqrt(2/3);
%         0, 0, 2*sqrt(2/3)
%     ];  % a unit double tetrahedron
%   
%   hydro_pos_accuracy is the machining tolerance of the hydrophone
%   mounts.
%   
%   Example:
%     hydro_pos_accuracy = 0.0000635;
%        % Machining tolerance of hydrophone mounts in meters
%        % 0.0000635 is 2.5 mils (thousandths of an inch)
%
%  tdoa_accuracy represents the accuracy with which the distance
%  difference between hydrophones and the receiver can be determined.
%
%  Example:
%    tdoa_accuracy = 0.01 / (2*pi) * 1500 / 30e3;
%    % A phase resolution of 0.01 radians, with a sound velocity of 1500
%    % meters per second, and a frequency of 30 kHz 

% If the matrix begins with a zero-filled row, strip it out
if all(hydro_pos(1,:)==[0,0,0])
  hydro_pos = hydro_pos(2:end,:);
end

m = size(hydro_pos);
m = m(1);

% Basic definitions %

hydro_to_source_vec = hydro_pos - repmat(source_pos, m, 1);
dists = sqrt(dot(hydro_to_source_vec, hydro_to_source_vec, 2));
dist0 = sqrt(dot(source_pos, source_pos));
ddoas = dists - dist0;
sigma_x = tdoa_accuracy;
sigma_s = hydro_pos_accuracy;

% Computing the Jacobian matrix %
% d(d10,d20,...,dm0,x1,y1,z1,x2,y2,z2,...,xm,ym,zm)
% -------------------------------------------------
%    d(xs,ys,zs,x1,y1,z1,x2,y2,z2,...,xm,ym,zm)

deriv11 = (repmat(source_pos,m,1)-hydro_pos)./repmat(dists,1,3)+repmat(source_pos,m,1)/dist0;

deriv12 = -(repmat(source_pos,m,1)-hydro_pos)./repmat(dists,1,3);
deriv12(:,4:((m+1)*3)) = 0;
deriv12 = reshape(transpose(deriv12),prod(size(deriv12)),1);
deriv12 = transpose(reshape(deriv12(1:(end-m*3)),3*m,m));

deriv21 = zeros(3*m,3);

deriv22 = eye(3*m);

deriv = zeros(4*m,3*(m+1));
deriv(1:m,1:3) = deriv11;
deriv(1:m,4:end) = deriv12;
deriv(m+1:end,1:3) = deriv21;
deriv(m+1:end,4:end) = deriv22;

% Computing the Cramer-Rao lower bound %

cov = zeros(4*m,4*m);
cov(1:m,1:m) = eye(m) * sigma_x^2 + (1 - eye(m)) * 0.5 * sigma_x^2;
cov(m+1:end,m+1:end) = eye(3*m) * sigma_s^2;

% Extracting and returning the x_s, y_s, z_s covariance matrix

cr_lowerbound = inv(transpose(deriv)*inv(cov)*deriv);
cr_lowerbound = cr_lowerbound(1:3,1:3);
