function Y = slidingSum(y, N)
Y = zeros(size(y));
Y(1) = 0;
for m=2:length(y)
  if m-N < 1
    Y(m) = (Y(m-1)+y(m));
  else
    Y(m) = (Y(m-1)-y(m-N)+y(m));
  end
end
