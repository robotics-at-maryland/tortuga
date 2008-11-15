function Y = slidingMedian(y,N)
Y = zeros(size(y));
len = length(y);
for i = 1:len
  medwindow = zeros(N,1);
  if i - N + 1 < 1
    lowerInd = 1;
  else
    lowerInd = i - N + 1;
  end
  medwindow(1:(i-lowerInd+1)) = y(lowerInd:i);
  Y(i) = median(medwindow);
end
end
