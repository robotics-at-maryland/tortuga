function M = strip_first_zeros(M)
% If the matrix begins with a zero-filled row, strip it out
if all(M(1,:)==[0,0,0])
  M = M(2:end,:);
end
