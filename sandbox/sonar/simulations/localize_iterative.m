function pos = localize_iterative(hydro_pos, tdoas, varargin)
% Find source position by one-step least squares algorithm, based on
% Ho's paper.

near_field = true;
num_iters = 2;
config;

hydro_pos = strip_first_zeros(hydro_pos);

% can be precomputed
m = size(hydro_pos);
m = m(1);

% can be precomputed
Qa = eye(m) * tdoa_accuracy^2 + 0.5 * (1 - eye(m)) * tdoa_accuracy^2;
Qb = tdoa_accuracy;

% can be precomputed
Rsq = dot(hydro_pos,hydro_pos,2);

G1 = hydro_pos;
G1(:,4) = tdoas;
G1 = -2 * G1;

h1 = tdoas' .^2 - Rsq;

% can be precomputed
W1 = inv(Qa);

th1 = inv(G1'*W1*G1)*G1'*W1*h1;

if near_field
  for i = 1:num_iters
    dists = hydro_pos - repmat(th1(1:3)',m,1);
    B1 = 2 * diag(sqrt(dot(dists,dists,2)));
    
    D1(1:m,1:3) = repmat(th1(1:3)',m,1);
    for j = 2:m
      D1(j-1,3*j-2:3*j) = th1(1:3)' - hydro_pos(j,:);
    end
    
    W1 = inv(B1*Qa*B1+D1*Qb*D1');
    
    th1 = inv(G1'*W1*G1)*G1'*W1*h1;
  end
end

covth1 = inv(G1'*W1*G1);

B2 = 2 * diag(th1);

D2 = zeros(4,3*m);
D2(4,1:3) = 2 * th1(1:3)';

h2 = th1 .^ 2;

W2 = inv(B2*covth1*B2 + D2*Qb*D2'+B2*inv(G1'*W1*G1)*G1'*W1*D1*Qb*D2'+...
         D2*Qb*D1'*W1*G1*inv(G1'*W1*G1)*B2');

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
    
    D2(4,1:3) = 2 * th';
    
    W2 = inv(B2*covth1*B2 + D2*Qb*D2'+B2*inv(G1'*W1*G1)*G1'*W1*D1*Qb*D2'+ ...
             D2*Qb*D1'*W1*G1*inv(G1'*W1*G1)*B2');
    
    th2 = inv(G2'*W2*G2)*G2'*W2*h2;
    % get rid of imaginary components
    th2 = max(th2,0);
    th = U .* sqrt(th2);
  end
end

B3 = B2(1:3,1:3);

covth2 = inv(G2'*W2*G2);

invB3 = inv(B3);
covth = invB3*covth2*invB3;

pos = th';
