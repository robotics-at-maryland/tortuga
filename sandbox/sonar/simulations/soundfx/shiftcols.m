function B = shiftcols(A,n)
% SHIFTCOLS(A,n)
%   Shift the columns of matrix A right by n, where n is any integer.
%   If n is positive, then each column moves right by n.
%   If n is negative, then each row moves left up by -n.
%   If n is zero, then no change occurs.
B = zeros(size(A));
if (n > 0)
    B(:,n+1:end) = A(:,1:(end-n));
elseif (n < 0)
    B(:,1:(end+n)) = A(:,-n+1:end);
else
    B = A;
end

