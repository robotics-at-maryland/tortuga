function B = shiftrows(A,n)
% SHIFTROWS(A,n)
%   Shift the rows of matrix A down by n, where n is any integer.
%   If n is positive, then each row moves down by n.
%   If n is negative, then each row moves up by -n.
%   If n is zero, then no change occurs.
B = zeros(size(A));
if (n > 0)
    B(n+1:end,:) = A(1:(end-n),:);
elseif (n < 0)
    B(1:(end+n),:) = A(-n+1:end,:);
else
    B = A;
end

