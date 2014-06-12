function [mu, sigma] = gaussFit(X, Y)
% Attempts to fit the ordered pairs X, Y to a gaussian curve with
% mean mu and standard deviation sigma.
  mu = sum(X .* Y) / sum(Y);
  sigma = sqrt(sum((mu - X).^2 .* Y) / sum(Y));
end
