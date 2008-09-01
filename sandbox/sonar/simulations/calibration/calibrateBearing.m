function Acal = calibrateBearing(numtrials)
	% Actual hydrohpone positions
	A = [0,1,0;1,0,0;0,0,-1] * 0.025;
	% Phase bias per channel (relative to zeroth channel)
	b = [0.001;0.002;0.003];
	
	% Wave vectors from a wide range of source bearings
	K = transpose(randUnitVector(numtrials, 3));
	
	% Measured TDOAs (actually DDOAs)
	D = inv(A) * K + repmat(b, 1, numtrials);
	
	% Augment V and X (a la OpenGL)
	K = vertcat(K, ones(1, numtrials));
	D = vertcat(D, ones(1, numtrials));
	Acal = K * pinv(D);
end

% Generate a set of random unit vectors
%
%  @param rows the number of vectors to create
%  @param cols the number of spatial dimensions (usually either 2 or 3)
%  @return a rows x cols matrix
function V = randUnitVector(rows, cols)
	V = randn(rows, cols);
	V = V ./ repmat(sqrt(dot(V,V,2)), 1, cols);
end
