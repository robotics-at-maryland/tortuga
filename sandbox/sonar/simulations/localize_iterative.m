function pos = localize_iterative(hydro_pos, tdoas)
% Find source position by one-step least squares algorithm, based on
% Huang's paper.

near_field = true;
num_iters = 2;
config;
varargin

% If the matrix begins with a zero-filled row, strip it out
if all(hydro_pos(1,:)==[0,0,0])
  hydro_pos = hydro_pos(2:end,:);
end

% can be precomputed
m = size(hydro_pos);
m = m(1);

% can be precomputed
Q = eye(m) * tdoa_accuracy^2 + 0.5 * (1 - eye(m)) * tdoa_accuracy^2;

% can be precomputed
Rsq = dot(hydro_pos,hydro_pos,2);

G1 = hydro_pos;
G1(:,4) = tdoas;
G1 = -2 * G1;

h1 = tdoas' .^2 - Rsq;

% can be precomputed
W1 = inv(Q);

th1 = inv(G1'*W1*G1)*G1'*W1*h1;

if near_field
  for i = 1:num_iters
    dists = hydro_pos - repmat(th1(1:3)',m,1);
    B1 = 2 * diag(sqrt(dot(dists,dists,2)));
    W1 = inv(B1*Q*B1');
    th1 = inv(G1'*W1*G1)*G1'*W1*h1;
  end
end

covth1 = inv(G1'*W1*G1);

B2 = 2 * diag(th1);

h2 = th1 .^ 2;

W2 = inv(B2 * covth1 * B2');

% can be precomputed
G2 = eye(3);
G2(4,:) = 1;

th2 = inv(G2'*W2*G2)*G2'*W2*h2;
% get rid of imaginary components
th2 = max(th2,0);

U = sign(th1(1:3));

th = U .* sqrt(th2);

if near_field
  for i = 1:num_iters
    B2 = 2 * diag(th);
    B2(4,4) = 2 * sqrt(dot(th,th));
    W2 = inv(B2*covth1*B2');
    th2 = inv(G2'*W2*G2)*G2'*W2*h2;
    % get rid of imaginary components
    th2 = max(th2,0);
    th = U .* sqrt(th2);
  end
end

B3 = B2(1:3,1:3);

covth2 = inv(G2'*W2*G2);

covth = inv(B3)*covth2*inv(B3');

pos = th';
