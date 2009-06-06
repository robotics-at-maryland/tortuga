function y=myIIR(x,A,B)
% Author: Scott Watson
% This is my own implementation of an IIR filter.  It takes as input a
% vector X representing sampled data, and it uses the A and B vectors
% to determine the transfer function(H(z)) used to produce the output Y.
%	     (A0) + (A1)z^-1 + ... + (Am)z^-N      A(z)
% H(z)= ----------------------------------  = ------
%        1 + (B1)z-1 + ... + (Bm)z^-N          B(z)
% Y(z)=X(z)*(A(z)/B(z))   -->   Y(z)B(z)=X(z)A(z)
% Y(z)=sum{k=0toN}{X(z)(Ak)z^-k} - sum{k=1toN}{Y(z)(Bk)z^-k}
% y[n]=sum{k=0toN}{x(n-k)(Ak)} - sum{k=1toN}{y(n-k)(Bk)}
%
% example usage...
% expected frequency response:
% freqz(conv([1 .9],[1 .9]),conv([1 -.8],[1 -.8]))
% test input:
% x=10*sin((0:.1:16*pi)*23)+sin(0:.1:16*pi)+2*sin((0:.1:16*pi)./8)
% command use:
% A=conv([1 .9],[1 .9]);B=conv([1 -.8],[1 -.8]);y=myIIR(x,A,B);

%zero pad the end of transfer function vectors to be of equal length
if size(A,2)>size(B,2)
	BB(1:size(A,2))=0;
	BB(1:size(B,2))=B;
	clear B
	B=BB;
elseif size(A,2)<size(B,2)
	AA(1:size(B,2))=0;
	AA(1:size(A,2))=A;
	clear A
	A=AA;	
end
% normalize so that lowest order coefficient of B vector is 1
A=A./B(1);
B=B./B(1);
% flip coeficient vectors to work well for matlab syntax
A=fliplr(A);
B=fliplr(B);
% get length of coeficients and input
NAB=size(A,2);
N=size(x,2);
% initialize output to all zeros
y(1:N)=0;
a(1:N)=0;
b(1:N)=0;
for n=1:N-NAB
	a(n+NAB)=sum(x(1+n:NAB+n).*A);				%zeros
	b(n+NAB)=sum(y(1+n:NAB-1+n).*B(1:NAB-1));	%poles
	y(n+NAB)=a(n+NAB)-b(n+NAB);		%zeros minus poles
end

hold off
plot(x,'b')		%plot of input
hold on
%plot(a,'ko')	%plot of intermediary result
%plot(b,'kx')	%plot of intermediary result
plot(y,'r')		%plot of output
