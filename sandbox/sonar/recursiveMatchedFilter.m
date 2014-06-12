function [ssqr, sdft, filterVal] = recursiveMatchedFilter(dat, f)

% ADC sample rate
fs = 500000;

% Pick the best k and N for the filter, with N between Nmin and Nmax.
Nmin = 480;
Nmax = 520;
Nchoices = Nmin:Nmax;
kChoices = f/fs.*Nchoices;
[dummy, Nchoice] = min(abs(round(kChoices)-kChoices));
k = kChoices(Nchoice);
N = Nchoices(Nchoice);

dat -= repmat(mean(dat,2),1,length(dat));

sdft = abs(slidingDFT(dat, k, N)).^2;
sqr  = dat.^2;
ssqr = slidingSum(sqr, N);

sdft = circshift(sdft, [0, -N]);

c1 = 60^2*N/sqrt(2);
c2 = c1 / 4;
c3 = (2000*N)^2;
c4 = 0.5*c3;
p1 = 0.5-0.5.*erf((ssqr-c1)./(2*c2));
p2 = 0.5+0.5.*erf((sdft-c3)./(2*c4));

filterVal = p1.*p2;

end
