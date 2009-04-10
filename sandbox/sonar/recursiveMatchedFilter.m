function [ssqr, sdft, filterVal] = recursiveMatchedFilter(dat, f)

% ADC sample rate
fs = 500000;

% Pick the best k and N for the filter, with N between Nmin and Nmax.
Nmin = 480;
Nmax = 520;
kChoice = min(abs(f/fs).*(Nmin:Nmax));
[k, N] = min(abs(kChoice - round(kChoice)));
N += Nmin - 1;

%dat -= repmat(mean(dat,2),1,length(dat));

sdft = abs(slidingDFT(dat, k, N)).^2;
sqr  = dat.^2;
ssqr = slidingSum(sqr, N);

sdft = circshift(sdft, [0, -N]);

c1 = 80^2*N/sqrt(2);
c2 = c1 / 4;
c3 = (10000*N/fs*f)^2;
c4 = 0.5*c3;
ssqr = 0.5+0.5.*tanh(-(sdft-c3)./(2*c4));
% exp(-ssqr./c1);
sdft = 0.5+0.5.*tanh((sdft-c3)./(2*c4));

filterVal = ssqr.*sdft;

end
