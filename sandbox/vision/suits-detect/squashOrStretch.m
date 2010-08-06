function W = squashOrStretch(V, newLength)
% W = squashOrStretch(V, newLength)
%   Resample a vector V so that it has length newLength.

oldLength = length(V);

if newLength == oldLength
	W = V;
elseif newLength < oldLength
	W = zeros(newLength,1);
	for i = 1:newLength
		W(i) = V(floor(i * oldLength / newLength));
	end
else # newLength > oldLength
	W = zeros(newLength,1);
	for i = 1:newLength
		lowIndex = floor(i * oldLength / newLength);
		highIndex = ceil(i * oldLength / newLength);
		interpParam = i * oldLength / newLength - lowIndex
		lowVal = V(lowIndex+1);
		highVal = V(highIndex);
		slope = (highVal - lowVal) / (highIndex - lowIndex);
		W(i) = slope * interpParam + lowVal;
	end
end
