function y=slidingINT2GoertzelDFT(x,k,N,precision)
% Sliding Goertzel DFT implemented with integer data types and even
% more speed fixes
% for more info see slidingGoertzelDFT()
% Author: Scott Watson
%
% Input parameter definitions
% x - input signal
% k - bin of interest  ( 0 <= k < N )
% N - size in samples of time window
SGcount=0; %3.27 seconds before overflow for a 16bit uint for 20ksps
v=0;
mem(1:2)=0;
y(1:size(x,2))=0;

for n=1:size(x,2)
  	[w yy result] = approxCoef(2*cos((2*pi*k)/N),precision);
 	coef1 = approxCoefEval(w,yy);
  	v = x(n) + mem(1)*coef1 - mem(2);
	%v = x(n) + mem(1)*2*cos((2*pi*k)/N) - mem(2);
	if SGcount>N+1
		v = v - x(n-N);
	end
	[w yy result] = approxCoef(cos((2*pi*k)/N),precision);
	costerm = approxCoefEval(w,yy);
	[w yy result] = approxCoef(sin((2*pi*k)/N),precision);
	sinterm = approxCoefEval(w,yy);
	y(n) = v - mem(1);
	y(n) = v - ( mem(1) * ( costerm + i*sinterm ) );
	out(n) = abs(costerm + i*sinterm);
	%y(n) = v - ( mem(1) * ( exp((i*2*pi*k)/N) ) );
	mem(2)=mem(1);		%incriment the delay line
	mem(1)=v;
	SGcount=SGcount+1;	%incriment buffer counter
end
%figure
%plot(out)