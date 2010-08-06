function Y = slidingDFT(y, k, N)
C = exp(-2*pi*i*k/N);
Y = zeros(size(y));
Y(1) = 0;
for m=2:length(y)
  if m-N < 1
    Y(m) = C * (Y(m-1)+y(m));
  else
    Y(m) = C * (Y(m-1)-y(m-N)+y(m));
  end
end
